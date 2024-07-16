
#include "physical_simulation_app.h"

namespace vulkancraft
{
	PhysicalSimulationApp::PhysicalSimulationApp()
	{
		try
		{
			init_physics_system();
			// test_create_dynamic_rigidbody();
		}
		catch (std::exception& e)
		{
			std::cerr << "物理系统初始化失败：" << e.what() << std::endl;
		}

		try
		{
			game_entity_manager_ = GameEntityManager::get_instance(); // 这里已经生成玩家了

			// 得到全局原子指针，这里进行阻塞以等待渲染线程创建完成
			while (glfw_window_ == nullptr)
			{
				glfw_window_ = global_glfw_window_ptr.load(std::memory_order_acquire);
				// std::cout << "物理线程正在试图获取 GLFW 窗口指针，如果持续看到该信息，则 GameWindow 类初始化失败" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("某个单例类初始化失败：" + std::string(e.what()));
		}

		// HACK: 此处初始化玩家物理系统
		game_entity_manager_->get_character_controller()->init_character_physics();
		register_player_physics(); // 注册玩家物理系统
		start_physical_thread();
	}

	PhysicalSimulationApp::~PhysicalSimulationApp()
	{
		// 确保线程在析构之前结束
		if (physics_thread_.joinable())
		{
			should_stop_ = true;
			physics_thread_.join();
		}

		glfw_window_ = nullptr; // 窗口指针直接设空就行，清除内容的工作在渲染线程内会由 GLFW 自主完成
		clear_physics_system(); // 清理物理系统
	}

	void PhysicalSimulationApp::init_physics_system()
	{
		collision_default_config_ = new btDefaultCollisionConfiguration();
		dispatcher_ = new btCollisionDispatcher(collision_default_config_);
		overlapping_pair_cache_ = new btDbvtBroadphase();
		solver_ = new btSequentialImpulseConstraintSolver();

		dynamics_world_ = new btDiscreteDynamicsWorld
		(
			dispatcher_,
			overlapping_pair_cache_,
			solver_,
			collision_default_config_
		);

		dynamics_world_->setGravity(kGravityVector);// 设置重力

		int thread_nums = std::thread::hardware_concurrency();

		if (thread_nums <= 0)
		{
			std::cout << "无法找到支持多线程运算的硬件" << std::endl;
		}
		else
		{
			dynamics_world_->setNumTasks(thread_nums);
		}
	}

	void PhysicalSimulationApp::clear_physics_system()
	{
		// remove the rigidbodies from the dynamics world and delete them
		for (int i = dynamics_world_->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamics_world_->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}

			dynamics_world_->removeCollisionObject(obj);
			delete obj;
		}

		// delete collision shapes
		for (int j = 0; j < physics_obj_map_.size(); j++)
		{
			btCollisionShape* shape = physics_obj_map_[j].collision_shape;
			physics_obj_map_[j].collision_shape = 0;
			delete shape;
		}

		// delete dynamics world
		delete dynamics_world_;

		// delete solver
		delete solver_;

		// delete broadphase
		delete overlapping_pair_cache_;

		// delete dispatcher
		delete dispatcher_;

		delete collision_default_config_;

		// next line is optional: it will be cleared by the destructor when the array goes out of scope
		physics_obj_map_.clear();
	}

	void PhysicalSimulationApp::start_physical_thread()
	{
		should_stop_ = false;
		physics_thread_ = std::thread(&PhysicalSimulationApp::update_physical_simulation, this);
	}

	void PhysicalSimulationApp::update_physical_simulation()
	{
		// HACK: 暂时让物理线程睡两秒，以后要加等待其他资源初始化完成的判断
		std::this_thread::sleep_for(std::chrono::seconds(2));

		std::cout << "物理 map 的大小为：" << physics_obj_map_.size() << std::endl;

		is_phy_sim_started = true;

		while (true)
		{
			// 计算追赶时间
			auto current_time = Clock::now(); // 记录现在的时间
			auto elapsed_time = current_time - previous_time_; // 自上一帧以来的经过时间
			previous_time_ = current_time; // 更新上一帧的时间点

			accumulator_ += elapsed_time.count(); // 累计时间差

			// 计算更新时间间隔
			auto current_step = Clock::now(); // 记录现在的时间
			auto elapsed_step = current_step - previous_step_; // 自上一次以来的间隔时间
			previous_step_ = current_step;

			float elapsed_step_float = static_cast<float>(elapsed_step.count()) / 1e9f;

			accumulator_step_ += elapsed_step.count(); // 累加间隔时间
			accumulator_delta_time_ = static_cast<float>(accumulator_step_) / 1e9f; // 物理间隔高精度纳秒转换回 float 类型，单位为秒

			// if (accumulator_step_ <= kTimeStep) continue; // 大锁：CD 时间没到，放弃追赶更新

			// 追赶更新
			while (accumulator_ >= kTimePerUpdate)
			{
				// std::cout << accumulator_step_ << std::endl;

				if (accumulator_step_ <= kTimeStep) break; // CD 时间没到就不循环

				// ==================== HACK 这下面是物理循环 ==================== //

				// 更新 Bullet 3 物理引擎的物理世界
				update_bullet_physics_world();

				// TODO: 更新玩家坐标位置

				// ==================== HACK 这上面是物理循环 ==================== //

				accumulator_ -= kTimePerUpdate; // 更新 accumulator（默认减去 1/60）
				accumulator_step_ = 0.0f; // 重置更新 CD 时间
			}

			// 帧率控制：计算剩余时间并延时
			double excess_time = accumulator_; // 这是累积的超出一个时间步的额外时间
			accumulator_ = std::fmod(accumulator_, kTimePerUpdate); // 更新 accumulator，去除完整的时间步，仅保留余数

			if (excess_time > 0)
			{
				// 如果有完整的时间步被处理掉（excess_time > 0），则不需要额外延时
				// 这里不需要sleep，因为已经在追赶更新中处理了时间
			}
			else
			{
				// 如果没有完整的时间步要处理（ 即 excess_time <= 0），则计算剩余到下一个更新的时间并延时
				double remaining_time = kTimePerUpdate - accumulator_;

				if (remaining_time > 0)
				{
					std::this_thread::sleep_for(std::chrono::duration<double>(remaining_time));
				}
				else
				{
					remaining_time = 0; // 确保 remaining_time 非负，避免潜在的负睡眠时间
				}
			}
		}
	}

	void PhysicalSimulationApp::update_bullet_physics_world()
	{
		dynamics_world_->stepSimulation(1.0f / 2000.0f, 10);

		for (int i = dynamics_world_->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamics_world_->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;

			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}

			game_entity_manager_->get_character_controller()->move(accumulator_delta_time_, glfw_window_);
			game_entity_manager_->get_character_controller()->sync_player_rigidbody_pos();

			// printf("world pos object %d = %f,%f,%f\n", i, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		}

		// std::cout << dynamics_world_->getCollisionObjectArray().size() << std::endl;
	}

#pragma region 物理游戏对象创建用函数

	void PhysicalSimulationApp::create_single_physics_block(BaseGameObject::id_t obj_id, PhysicsObjectCreateData data)
	{
		std::lock_guard<std::mutex> lock(this->collision_shape_array_mutex_);

		btCollisionShape* single_block = new btBoxShape(data.obj_size); // 确定大小
		data.transform.setIdentity();
		bool is_dynamic = (data.mass != 0.0f);

		// 如果是动态物体，设置受力张量矩阵
		if (is_dynamic) single_block->calculateLocalInertia(data.mass, data.local_inertia);
		data.transform.setOrigin(data.obj_origin);

		btDefaultMotionState* block_motion_state = new btDefaultMotionState(data.transform);
		btRigidBody::btRigidBodyConstructionInfo rb_info(data.mass, block_motion_state, single_block, data.local_inertia);
		btRigidBody* body = new btRigidBody(rb_info);

		PhysicsObjectSaveData new_data =
		{
			single_block,
			body
		};

		this->physics_obj_map_.emplace(obj_id, new_data);
		dynamics_world_->addRigidBody(body);
	}

	void PhysicalSimulationApp::register_player_physics()
	{
		PhysicsObjectSaveData player_data = game_entity_manager_->get_character_controller()->get_player_physics_data();
		BaseGameObject::id_t player_id = game_entity_manager_->get_character_controller()->get_player_obj_id();

		this->physics_obj_map_.emplace(player_id, player_data);
		dynamics_world_->addRigidBody(player_data.rigid_body);
	}

#pragma endregion

#pragma region DEBUG 用函数

	void PhysicalSimulationApp::test_create_dynamic_rigidbody()
	{
		// 创建测试用的物理物体，这个基本就是对着官方的抄一遍
		btCollisionShape* ground_shape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		btTransform ground_transform;
		ground_transform.setIdentity();
		ground_transform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.0f); // 设置地板的质量，小于 0 那它就是静态的

		// 当且仅当质量不是零，刚体是动态的，否则是静态的
		bool is_dynamic = (mass != 0.f);

		btVector3 local_inertia(0, 0, 0);

		if (is_dynamic) ground_shape->calculateLocalInertia(mass, local_inertia);

		// 使用 motionstate 是可选的，它提供插值功能，并且只同步 “活动” 对象
		btDefaultMotionState* my_motion_state = new btDefaultMotionState(ground_transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, my_motion_state, ground_shape, local_inertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		PhysicsObjectSaveData test_ground_data = 
		{
			ground_shape,
			body
		};

		// HACK: 这里用掉了一个键
		this->physics_obj_map_.emplace(123000, test_ground_data);

		// 注册刚体
		dynamics_world_->addRigidBody(body);

		// 创建动态物体
		{
			btCollisionShape* test_cube = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

			btTransform start_transform;
			start_transform.setIdentity();

			btScalar mass(1.0f);
			bool is_dynamic = (mass != 0.f);

			btVector3 local_inertia(0, 0, 0);
			if (is_dynamic) test_cube->calculateLocalInertia(mass, local_inertia);

			start_transform.setOrigin(btVector3(2, 10, 0));

			btDefaultMotionState* my_motion_state = new btDefaultMotionState(start_transform);
			btRigidBody::btRigidBodyConstructionInfo rb_info(mass, my_motion_state, test_cube, local_inertia);
			btRigidBody* body = new btRigidBody(rb_info);

			PhysicsObjectSaveData test_cube_data =
			{
				test_cube,
				body
			};

			// HACK: 这里用掉了一个键
			this->physics_obj_map_.emplace(123001, test_cube_data);
			dynamics_world_->addRigidBody(body);
		}
	}

#pragma endregion

}
