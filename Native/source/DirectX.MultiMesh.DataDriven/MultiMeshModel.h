#pragma once
#include "MultiMeshModel.h"
#include "pch.h"



namespace MultiMesh {
	class MultiMeshModel :
		public Library::DrawableGameComponent
	{
	public:
		MultiMeshModel(Library::Game& game);
		MultiMeshModel(Library::Game& game, const std::shared_ptr<Library::Camera>& camera);

		virtual void Initialize() override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		virtual void Update(const Library::GameTime& gameTime) override;
	private:
		std::vector<UINT> offsets;


	};

}