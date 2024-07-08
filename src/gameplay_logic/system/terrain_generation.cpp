
#include "terrain_generation.h"

namespace vulkancraft
{
	TerrainGeneration::TerrainGeneration(GameDevice& game_device, RenderAppObjMap& game_object_map) : game_device_{ game_device }, game_object_map_{ game_object_map }
	{
		try
		{
			game_object_manager_ = GameObjectManager::get_instance();
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("ĳ���������ʼ��ʧ�ܣ�" + std::string(e.what()));
		}
	}

	TerrainGeneration::~TerrainGeneration() { }

	void TerrainGeneration::single_block_creator(BlockGenerateData block_data)
	{
		std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, model_file_path_);
		BaseGameObject stone_obj = BaseGameObject::create_game_object(false);

		stone_obj.model_ = stone_model;
		stone_obj.transform_.translation = block_data.position;
		stone_obj.transform_.rotation = block_data.rotation;
		stone_obj.transform_.scale = block_data.scale;

		// ���ɷ���� Box Collider
		AABBCollider collider =
		{
			false,
			stone_obj.transform_,
			stone_obj.get_id(),
			true,
			0.0f,
			0.0f
		};

		// ���ɷ���ȫ�ֹ�������
		GameObjectPublicData stone_obj_public_data =
		{
			stone_obj.get_id(),
			stone_obj.get_static_state(),
			stone_obj.transform_,
			collider
		};

		// �������ݷ��� game object manager
		game_object_manager_->insert_sharing_game_object_data(stone_obj.get_id(), stone_obj_public_data);

		if (!stone_obj.get_static_state()) // �Ǿ�̬������Ҫ�����������
		{
			game_object_manager_->add_physical_obj_by_id(stone_obj.get_id());
		}

		// ��Ⱦ���ݷ��� game object map
		game_object_map_.emplace(stone_obj.get_id(), std::move(stone_obj));
	}

	void TerrainGeneration::create_plane(int length, int width)
	{
		BlockGenerateData new_data =
		{
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }
		};

		new_data.position.x = 0;

		for (int j = 0; j < width; j++) // ���������һ��
		{
			single_block_creator(new_data);
			new_data.position.z += 1;
		}

		new_data.position.z = 0;

		for (int i = 0; i < length; i++) // ���� plane �ı߳�
		{
			single_block_creator(new_data);
			new_data.position.x += 1;

			for (int j = 0; j < width; j++)
			{
				single_block_creator(new_data);
				new_data.position.z += 1;
			}

			new_data.position.z = 0;
		}
	}

	void TerrainGeneration::create_wall(int height, int width)
	{
		BlockGenerateData new_data =
		{
			{ 4.0f, 0.0f, 8.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }
		};

		new_data.position.x = 4.0f;

		for (int j = 0; j < width; j++) // ���������һ��
		{
			single_block_creator(new_data);
			new_data.position.y -= 1;
		}

		new_data.position.y = 0;

		for (int i = 0; i < height; i++) // ���� plane �ı߳�
		{
			single_block_creator(new_data);
			new_data.position.x += 1;

			for (int j = 0; j < width; j++)
			{
				single_block_creator(new_data);
				new_data.position.y -= 1;
			}

			new_data.position.y = 0;
		}
	}

#pragma region DEBUG �ú���

	void TerrainGeneration::test_dynamic_block_create(BlockGenerateData block_data)
	{
		std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, model_file_path_);
		BaseGameObject stone_obj = BaseGameObject::create_game_object(block_data.is_static);

		stone_obj.model_ = stone_model;
		stone_obj.transform_.translation = block_data.position;
		stone_obj.transform_.rotation = block_data.rotation;
		stone_obj.transform_.scale = block_data.scale;
	}

#pragma endregion

}
