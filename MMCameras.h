//=============================================================
//
// Tombstone Engine version 1.0
// Copyright 2016, by Terathon Software LLC
//
// This file is part of the Tombstone Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef MMCameras_h
#define MMCameras_h


#include "TSCameras.h"
#include "TSModels.h"
#include "MMBase.h"


namespace MMGame
{
	using namespace Tombstone;


	extern const float kCameraPositionHeight;


	class ModelCamera : public FrustumCamera
	{
		private:

			Model		*targetModel;

		protected:

			ModelCamera();

		public:

			~ModelCamera();

			Model *GetTargetModel(void) const
			{
				return (targetModel);
			}

			void SetTargetModel(Model *model)
			{
				targetModel = model;
			}
	};


	class FirstPersonCamera final : public ModelCamera
	{
		public:

			FirstPersonCamera();
			~FirstPersonCamera();

			void MoveCamera(void) override;


			void setMovementFlag(int);
	};


	class ChaseCamera final : public ModelCamera
	{
		public:

			ChaseCamera();
			~ChaseCamera();

			void MoveCamera(void) override;
	};
}


#endif

// ZUXSVMT
