
#include "terrain_generation.h"

namespace vulkancraft
{
	TerrainGeneration::TerrainGeneration(GameDevice& game_device, BaseGameObject::Map& game_object_map) : game_device_ {game_device}, game_object_map_ {game_object_map}
	{
		try
		{
			game_object_manager_ = GameObjectManager::get_instance();
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("某个单例类初始化失败：" + std::string(e.what()));
		}
	}

	TerrainGeneration::~TerrainGeneration() { }

	void TerrainGeneration::single_block_creator(BlockGenerateData block_data)
	{
		std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, model_file_path_);
		BaseGameObject stone_obj = BaseGameObject::create_game_object(false);

		stone_obj.model_ = stone_model;
		stone_obj.transform_.translation = block_data.position;
		stone_obj.transform_.rotation = block_data.position;
		stone_obj.transform_.scale = block_data.scale;

		// 生成方块的 Box Collider
		AABBCollider collider =
		{
			false,
			stone_obj.transform_,
			stone_obj.get_id(),
			true,
			0.0f,
			0.0f
		};

		// 生成方块全局共享数据
		GameObjectPublicData stone_obj_public_data =
		{
			stone_obj.get_id(),
			stone_obj.get_static_state(),
			stone_obj.transform_,
			collider
		};

		// 公共数据放入 game object manager
		game_object_manager_->insert_sharing_game_object_data(stone_obj.get_id(), stone_obj_public_data);

		if (!stone_obj.get_static_state()) // 非静态物体需要进行物理计算
		{
			game_object_manager_->add_physical_obj_by_id(stone_obj.get_id());
		}

		// 渲染数据放入 game object map
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

		for (int i = 0; i < length; i++) // 对于 plane 的边长
		{
			for (int i = 0; i < width; i++) // 对于每个边长单位都生成这么多宽的方块
			{
				single_block_creator(new_data);

				new_data.position.x += 1;
				new_data.position.z += 1; // 更新数据
			}
		}
	}

}
