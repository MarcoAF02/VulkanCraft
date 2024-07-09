
#include "physical_simulation_app.h"

namespace vulkancraft
{
	PhysicalSimulationApp::PhysicalSimulationApp()
	{
		try
		{
			init_physics_system();
			test_create_dynamic_rigidbody();
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

		update_physical_simulation(); // ��������ģ��ѭ��
	}

	PhysicalSimulationApp::~PhysicalSimulationApp()
	{
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
		for (int j = 0; j < collision_shape_array_.size(); j++)
		{
			btCollisionShape* shape = collision_shape_array_[j];
			collision_shape_array_[j] = 0;
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
		collision_shape_array_.clear();
	}

	void PhysicalSimulationApp::update_physical_simulation()
	{
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

			accumulator_step_ += elapsed_step.count(); // �ۼӼ��ʱ��
			float accumulator_delta_time = static_cast<float>(accumulator_step_) / 1e9f; // �������߾�������ת���� float ���ͣ���λΪ��

			// if (accumulator_step_ <= kTimeStep) continue; // ������CD ʱ��û��������׷�ϸ���

			// ׷�ϸ���
			while (accumulator_ >= kTimePerUpdate)
			{
				// std::cout << accumulator_step_ << std::endl;

				if (accumulator_step_ <= kTimeStep) break; // CD ʱ��û���Ͳ�ѭ��

				// ==================== HACK ������������ѭ�� ==================== //

				// calculate_aabb_collider(); // ������⶯̬����� Collider �Ƿ�����ײ

				// ���� Bullet 3 �������洴������������
				// ����ײ���������ȶ��������������ʵ��
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

			// printf("world pos object %d = %f,%f,%f\n", i, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		}
	}

#pragma region ������Ϸ���󴴽��ú���

	void PhysicalSimulationApp::create_single_physics_block(PhysicsObjectCreateData data)
	{
		std::lock_guard<std::mutex> lock(this->collision_shape_array_mutex_);

		// TODO: �� map ��¼�����Ϸ����
		btCollisionShape* single_block = new btBoxShape(data.obj_size); // ȷ����С

		// TODO: ������ʲ���
		this->collision_shape_array_.push_back(single_block);

		data.transform.setIdentity();
		bool is_dynamic = (data.mass != 0.0f);

		// ����Ƕ�̬���壬����������������
		if (is_dynamic) single_block->calculateLocalInertia(data.mass, data.local_inertia);

		data.transform.setOrigin(data.obj_origin);

		btDefaultMotionState* block_motion_state = new btDefaultMotionState(data.transform);
		btRigidBody::btRigidBodyConstructionInfo rb_info(data.mass, block_motion_state, single_block, data.local_inertia);
		btRigidBody* body = new btRigidBody(rb_info);

		dynamics_world_->addRigidBody(body);
	}

#pragma endregion

#pragma region DEBUG �ú���

	void PhysicalSimulationApp::test_create_dynamic_rigidbody()
	{
		// ���������õ��������壬����������Ƕ��Źٷ��ĳ�һ��
		btCollisionShape* ground_shape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		collision_shape_array_.push_back(ground_shape);

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

		// ע�����
		dynamics_world_->addRigidBody(body);

		// ������̬����
		{
			btCollisionShape* test_cube = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
			collision_shape_array_.push_back(test_cube);

			btTransform start_transform;
			start_transform.setIdentity();

			btScalar mass(1.f);
			bool is_dynamic = (mass != 0.f);

			btVector3 local_inertia(0, 0, 0);
			if (is_dynamic) test_cube->calculateLocalInertia(mass, local_inertia);

			start_transform.setOrigin(btVector3(2, 10, 0));

			btDefaultMotionState* my_motion_state = new btDefaultMotionState(start_transform);
			btRigidBody::btRigidBodyConstructionInfo rb_info(mass, my_motion_state, test_cube, local_inertia);
			btRigidBody* body = new btRigidBody(rb_info);

			dynamics_world_->addRigidBody(body);
		}
	}

#pragma endregion

}
