
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
			std::cerr << "����ϵͳ��ʼ��ʧ�ܣ�" << e.what() << std::endl;
		}

		try
		{
			game_entity_manager_ = GameEntityManager::get_instance(); // �����Ѿ����������

			// �õ�ȫ��ԭ��ָ�룬������������Եȴ���Ⱦ�̴߳������
			while (glfw_window_ == nullptr)
			{
				glfw_window_ = global_glfw_window_ptr.load(std::memory_order_acquire);
				// std::cout << "�����߳�������ͼ��ȡ GLFW ����ָ�룬���������������Ϣ���� GameWindow ���ʼ��ʧ��" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("ĳ���������ʼ��ʧ�ܣ�" + std::string(e.what()));
		}

		// HACK: �˴���ʼ���������ϵͳ
		game_entity_manager_->get_character_controller()->init_character_physics();
		register_player_physics(); // ע���������ϵͳ
		start_physical_thread();
	}

	PhysicalSimulationApp::~PhysicalSimulationApp()
	{
		// ȷ���߳�������֮ǰ����
		if (physics_thread_.joinable())
		{
			should_stop_ = true;
			physics_thread_.join();
		}

		glfw_window_ = nullptr; // ����ָ��ֱ����վ��У�������ݵĹ�������Ⱦ�߳��ڻ��� GLFW �������
		clear_physics_system(); // ��������ϵͳ
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

		dynamics_world_->setGravity(kGravityVector);// ��������

		int thread_nums = std::thread::hardware_concurrency();

		if (thread_nums <= 0)
		{
			std::cout << "�޷��ҵ�֧�ֶ��߳������Ӳ��" << std::endl;
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
		// HACK: ��ʱ�������߳�˯���룬�Ժ�Ҫ�ӵȴ�������Դ��ʼ����ɵ��ж�
		std::this_thread::sleep_for(std::chrono::seconds(2));

		std::cout << "���� map �Ĵ�СΪ��" << physics_obj_map_.size() << std::endl;

		is_phy_sim_started = true;

		while (true)
		{
			// ����׷��ʱ��
			auto current_time = Clock::now(); // ��¼���ڵ�ʱ��
			auto elapsed_time = current_time - previous_time_; // ����һ֡�����ľ���ʱ��
			previous_time_ = current_time; // ������һ֡��ʱ���

			accumulator_ += elapsed_time.count(); // �ۼ�ʱ���

			// �������ʱ����
			auto current_step = Clock::now(); // ��¼���ڵ�ʱ��
			auto elapsed_step = current_step - previous_step_; // ����һ�������ļ��ʱ��
			previous_step_ = current_step;

			float elapsed_step_float = static_cast<float>(elapsed_step.count()) / 1e9f;

			accumulator_step_ += elapsed_step.count(); // �ۼӼ��ʱ��
			accumulator_delta_time_ = static_cast<float>(accumulator_step_) / 1e9f; // �������߾�������ת���� float ���ͣ���λΪ��

			// if (accumulator_step_ <= kTimeStep) continue; // ������CD ʱ��û��������׷�ϸ���

			// ׷�ϸ���
			while (accumulator_ >= kTimePerUpdate)
			{
				// std::cout << accumulator_step_ << std::endl;

				if (accumulator_step_ <= kTimeStep) break; // CD ʱ��û���Ͳ�ѭ��

				// ==================== HACK ������������ѭ�� ==================== //

				// ���� Bullet 3 �����������������
				update_bullet_physics_world();

				// TODO: �����������λ��

				// ==================== HACK ������������ѭ�� ==================== //

				accumulator_ -= kTimePerUpdate; // ���� accumulator��Ĭ�ϼ�ȥ 1/60��
				accumulator_step_ = 0.0f; // ���ø��� CD ʱ��
			}

			// ֡�ʿ��ƣ�����ʣ��ʱ�䲢��ʱ
			double excess_time = accumulator_; // �����ۻ��ĳ���һ��ʱ�䲽�Ķ���ʱ��
			accumulator_ = std::fmod(accumulator_, kTimePerUpdate); // ���� accumulator��ȥ��������ʱ�䲽������������

			if (excess_time > 0)
			{
				// �����������ʱ�䲽���������excess_time > 0��������Ҫ������ʱ
				// ���ﲻ��Ҫsleep����Ϊ�Ѿ���׷�ϸ����д�����ʱ��
			}
			else
			{
				// ���û��������ʱ�䲽Ҫ���� �� excess_time <= 0���������ʣ�ൽ��һ�����µ�ʱ�䲢��ʱ
				double remaining_time = kTimePerUpdate - accumulator_;

				if (remaining_time > 0)
				{
					std::this_thread::sleep_for(std::chrono::duration<double>(remaining_time));
				}
				else
				{
					remaining_time = 0; // ȷ�� remaining_time �Ǹ�������Ǳ�ڵĸ�˯��ʱ��
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

#pragma region ������Ϸ���󴴽��ú���

	void PhysicalSimulationApp::create_single_physics_block(BaseGameObject::id_t obj_id, PhysicsObjectCreateData data)
	{
		std::lock_guard<std::mutex> lock(this->collision_shape_array_mutex_);

		btCollisionShape* single_block = new btBoxShape(data.obj_size); // ȷ����С
		data.transform.setIdentity();
		bool is_dynamic = (data.mass != 0.0f);

		// ����Ƕ�̬���壬����������������
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

#pragma region DEBUG �ú���

	void PhysicalSimulationApp::test_create_dynamic_rigidbody()
	{
		// ���������õ��������壬����������Ƕ��Źٷ��ĳ�һ��
		btCollisionShape* ground_shape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		btTransform ground_transform;
		ground_transform.setIdentity();
		ground_transform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.0f); // ���õذ��������С�� 0 �������Ǿ�̬��

		// ���ҽ������������㣬�����Ƕ�̬�ģ������Ǿ�̬��
		bool is_dynamic = (mass != 0.f);

		btVector3 local_inertia(0, 0, 0);

		if (is_dynamic) ground_shape->calculateLocalInertia(mass, local_inertia);

		// ʹ�� motionstate �ǿ�ѡ�ģ����ṩ��ֵ���ܣ�����ֻͬ�� ����� ����
		btDefaultMotionState* my_motion_state = new btDefaultMotionState(ground_transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, my_motion_state, ground_shape, local_inertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		PhysicsObjectSaveData test_ground_data = 
		{
			ground_shape,
			body
		};

		// HACK: �����õ���һ����
		this->physics_obj_map_.emplace(123000, test_ground_data);

		// ע�����
		dynamics_world_->addRigidBody(body);

		// ������̬����
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

			// HACK: �����õ���һ����
			this->physics_obj_map_.emplace(123001, test_cube_data);
			dynamics_world_->addRigidBody(body);
		}
	}

#pragma endregion

}
