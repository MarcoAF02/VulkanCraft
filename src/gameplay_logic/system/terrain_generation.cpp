
#include "terrain_generation.h"

namespace vulkancraft
{
	TerrainGeneration::TerrainGeneration() { }
	TerrainGeneration::~TerrainGeneration() { }

	void TerrainGeneration::single_block_creator(BlockGenerateData block_data)
	{
		//// TODO: 
		//// 1. 创建渲染 game object
		//// 2. 创建物理 game object
		//// 3. 两个线程的 ID 要同步
		//// 4. 物理线程 APP 数据同步回渲染线程 APP 表现变化效果
		//std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, model_file_path_);
		//BaseGameObject stone_obj = BaseGameObject::create_game_object(false);

		//stone_obj.model_ = stone_model;
		//stone_obj.transform_.translation = block_data.position;
		//stone_obj.transform_.rotation = block_data.rotation;
		//stone_obj.transform_.scale = block_data.scale;

		//btVector3 obj_size{ block_data.scale.x, block_data.scale.y, block_data.scale.z };
		//btVector3 obj_origin{ block_data.position.x, block_data.position.y, block_data.position.z };
		//float obj_mass = block_data.mass;

		//// 生成方块物理数据
		//PhysicsObjectCreateData obj_data = // 不知道传什么就打花括号
		//{
		//	obj_size,
		//	obj_origin,
		//	{},
		//	obj_mass,
		//	{}
		//};

		//// 渲染数据放入 game object map
		//game_object_map_.emplace(stone_obj.get_id(), std::move(stone_obj));

		//// TODO: 通知 PhysicsObjectGenerator 类
		//physical_simulation_app_.create_single_physics_block(obj_data);
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

		for (int j = 0; j < width; j++) // 单独处理第一列
		{
			single_block_creator(new_data);
			new_data.position.z += 1;
		}

		new_data.position.z = 0;

		for (int i = 0; i < length; i++) // 对于 plane 的边长
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

		for (int j = 0; j < width; j++) // 单独处理第一列
		{
			single_block_creator(new_data);
			new_data.position.y -= 1;
		}

		new_data.position.y = 0;

		for (int i = 0; i < height; i++) // 对于 plane 的边长
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

}
