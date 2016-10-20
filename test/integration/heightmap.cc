/*
 * Copyright (C) 2012-2016 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <string.h>
#include <ignition/math/Vector3.hh>

#include "gazebo/common/SystemPaths.hh"
#include "gazebo/rendering/RenderingIface.hh"
#include "gazebo/rendering/Scene.hh"
#include "heights_cmp.h"
#include "gazebo/test/helper_physics_generator.hh"
#include "images_cmp.h"
#include "gazebo/test/ServerFixture.hh"

using namespace gazebo;

class HeightmapTest : public ServerFixture,
                      public testing::WithParamInterface<const char*>
{
  public: void PhysicsLoad(const std::string &_physicsEngine);
  public: void WhiteAlpha(const std::string &_physicsEngine);
  public: void WhiteNoAlpha(const std::string &_physicsEngine);
  public: void Volume(const std::string &_physicsEngine);
  public: void LoadDEM(const std::string &_physicsEngine);
  public: void NotSquareImage();
  public: void InvalidSizeImage();
  // public: void Heights(const std::string &_physicsEngine);
};

/////////////////////////////////////////////////
void HeightmapTest::PhysicsLoad(const std::string &_physicsEngine)
{
  if (_physicsEngine == "dart")
  {
    gzerr << "Aborting test for dart, see issue #909" << std::endl;
    return;
  }

  Load("worlds/heightmap_test.world", true, _physicsEngine);

  // Make sure the render engine is available.
  if (rendering::RenderEngine::Instance()->GetRenderPathType() ==
      rendering::RenderEngine::NONE)
  {
    gzerr << "No rendering engine, unable to run heights test\n";
    return;
  }

  physics::ModelPtr model = GetModel("heightmap");
  EXPECT_TRUE(model != NULL);

  physics::CollisionPtr collision =
    model->GetLink("link")->GetCollision("collision");

  physics::HeightmapShapePtr shape =
    boost::dynamic_pointer_cast<physics::HeightmapShape>(
        collision->GetShape());

  EXPECT_TRUE(shape != NULL);
  EXPECT_TRUE(shape->HasType(physics::Base::HEIGHTMAP_SHAPE));

  EXPECT_TRUE(shape->GetPos() == ignition::math::Vector3d(0, 0, 0));
  EXPECT_TRUE(shape->GetSize() == ignition::math::Vector3d(129, 129, 10));

  common::Image trueImage("media/materials/textures/heightmap_bowl.png");
  common::Image testImage = shape->GetImage();

  common::SystemPaths *paths = common::SystemPaths::Instance();
  testImage.SavePNG(paths->GetTmpPath() + "/test_shape.png");

  EXPECT_EQ(trueImage.GetWidth(), testImage.GetWidth());
  EXPECT_EQ(trueImage.GetHeight(), testImage.GetHeight());

  // Debug output
  // Compare the true image to the image generated by the heightmap shape
  // for (uint16_t y = 0; y < testImage.GetHeight(); y += 1.0)
  // {
  //   for (uint16_t x = 0; x < testImage.GetWidth(); x += 1.0)
  //   {
  //     // EXPECT_NEAR(trueImage.GetPixel(x, y).r,
  //     //             testImage.GetPixel(x, y).r, 0.008);
  //     // if (fabs(trueImage.GetPixel(x, y).r - testImage.GetPixel(x, y).r)
  //     // > 0.008)
  //     {
  //       printf("XY[%d %d] True[%f] Test[%f]\n", x, y,
  //           trueImage.GetPixel(x, y).r, testImage.GetPixel(x, y).r);
  //     }
  //   }
  // }
}

/////////////////////////////////////////////////
void HeightmapTest::WhiteAlpha(const std::string &_physicsEngine)
{
  if (_physicsEngine == "dart")
  {
    gzerr << "Aborting test for dart, see issue #909" << std::endl;
    return;
  }

  Load("worlds/white_alpha_heightmap.world", true, _physicsEngine);
  physics::ModelPtr model = GetModel("heightmap");
  EXPECT_TRUE(model != NULL);

  physics::CollisionPtr collision =
    model->GetLink("link")->GetCollision("collision");

  physics::HeightmapShapePtr shape =
    boost::dynamic_pointer_cast<physics::HeightmapShape>(collision->GetShape());

  EXPECT_TRUE(shape != NULL);
  EXPECT_TRUE(shape->HasType(physics::Base::HEIGHTMAP_SHAPE));

  int x, y;
  for (y = 0; y < shape->GetVertexCount().y; ++y)
  {
    for (x = 0; x < shape->GetVertexCount().x; ++x)
    {
      EXPECT_NEAR(shape->GetHeight(x, y), 10.0, 1e-4);
    }
  }
}

/////////////////////////////////////////////////
void HeightmapTest::WhiteNoAlpha(const std::string &_physicsEngine)
{
  if (_physicsEngine == "dart")
  {
    gzerr << "Aborting test for dart, see issue #909" << std::endl;
    return;
  }

  Load("worlds/white_no_alpha_heightmap.world", true, _physicsEngine);
  physics::ModelPtr model = GetModel("heightmap");
  EXPECT_TRUE(model != NULL);

  physics::CollisionPtr collision =
    model->GetLink("link")->GetCollision("collision");

  physics::HeightmapShapePtr shape =
    boost::dynamic_pointer_cast<physics::HeightmapShape>(collision->GetShape());

  EXPECT_TRUE(shape != NULL);
  EXPECT_TRUE(shape->HasType(physics::Base::HEIGHTMAP_SHAPE));

  int x, y;
  for (y = 0; y < shape->GetVertexCount().y; ++y)
  {
    for (x = 0; x < shape->GetVertexCount().x; ++x)
    {
      EXPECT_EQ(shape->GetHeight(x, y), 10.0);
    }
  }
}

/////////////////////////////////////////////////
void HeightmapTest::NotSquareImage()
{
  common::SystemPaths::Instance()->AddGazeboPaths(
      TEST_INTEGRATION_PATH);

  this->server = new Server();
  this->server->PreLoad();
  // EXPECT_THROW(this->server->LoadFile("worlds/not_square_heightmap.world"),
  //            common::Exception);

  this->server->Fini();
  delete this->server;
}

/////////////////////////////////////////////////
void HeightmapTest::InvalidSizeImage()
{
  common::SystemPaths::Instance()->AddGazeboPaths(
      TEST_INTEGRATION_PATH);

  this->server = new Server();
  this->server->PreLoad();
  // EXPECT_THROW(this->server->LoadFile("worlds/invalid_size_heightmap.world"),
  //             common::Exception);

  this->server->Fini();
  delete this->server;
}

/////////////////////////////////////////////////
void HeightmapTest::Volume(const std::string &_physicsEngine)
{
  if (_physicsEngine == "simbody")
  {
    // SimbodyHeightmapShape unimplemented. ComputeVolume actually returns 0 as
    // an error code, which is the correct answer, but we'll skip it for now.
    gzerr << "Aborting test for "
          << _physicsEngine
          << std::endl;
    return;
  }
  if (_physicsEngine == "dart")
  {
    gzerr << "Aborting test for "
          << _physicsEngine
          << ", see issue #909" << std::endl;
    return;
  }

  Load("worlds/heightmap_test.world", true, _physicsEngine);

  physics::ModelPtr model = GetModel("heightmap");
  EXPECT_TRUE(model != NULL);

  physics::CollisionPtr collision =
    model->GetLink("link")->GetCollision("collision");

  physics::HeightmapShapePtr shape =
    boost::dynamic_pointer_cast<physics::HeightmapShape>(
        collision->GetShape());

  EXPECT_DOUBLE_EQ(shape->ComputeVolume(), 0);
}

/////////////////////////////////////////////////
void HeightmapTest::LoadDEM(const std::string &_physicsEngine)
{
#ifdef HAVE_GDAL
  if (_physicsEngine == "dart")
  {
    gzerr << "Aborting test for dart, see issue #909" << std::endl;
    return;
  }

  if (_physicsEngine == "bullet" || _physicsEngine == "simbody")
  {
    gzerr << "Aborting test for " << _physicsEngine <<
        ", negative elevations are not working yet." << std::endl;
    return;
  }

  Load("worlds/dem_neg.world", true, _physicsEngine);

  physics::WorldPtr world = physics::get_world("default");
  ASSERT_TRUE(world != nullptr);

  physics::ModelPtr boxModel = GetModel("box");
  EXPECT_TRUE(boxModel != nullptr);

  ignition::math::Pose3d boxInitPose(0, 0, -207, 0, 0, 0);
  EXPECT_EQ(boxModel->GetWorldPose().Ign(), boxInitPose);

  physics::ModelPtr model = GetModel("heightmap");
  EXPECT_TRUE(model != nullptr);

  physics::CollisionPtr collision =
    model->GetLink("link")->GetCollision("collision");

  physics::HeightmapShapePtr shape =
    boost::dynamic_pointer_cast<physics::HeightmapShape>(
        collision->GetShape());

  EXPECT_TRUE(shape != NULL);
  EXPECT_TRUE(shape->HasType(physics::Base::HEIGHTMAP_SHAPE));

  EXPECT_TRUE(shape->GetPos() == ignition::math::Vector3d(0, 0, 0));

  double maxHeight = shape->GetMaxHeight();
  double minHeight = shape->GetMinHeight();
  EXPECT_GE(maxHeight, minHeight);
  EXPECT_GE(boxInitPose.Pos().Z(), minHeight);

  // step the world
  // let the box fall onto the heightmap and wait for it to rest
  world->Step(1000);

  ignition::math::Pose3d boxRestPose = boxModel->GetWorldPose().Ign();
  EXPECT_NE(boxRestPose, boxInitPose);
  EXPECT_GE(boxInitPose.Pos().Z(), minHeight);

  // step the world and verify the box is at rest
  world->Step(100);

  ignition::math::Pose3d boxNewRestPose = boxModel->GetWorldPose().Ign();
  EXPECT_EQ(boxNewRestPose, boxRestPose);
#else
  // prevent unused variable warning
  (void)(_physicsEngine);
#endif
}

/*
void HeightmapTest::Heights(const std::string &_physicsEngine)
{
  Load("worlds/heightmap_test.world", _physicsEngine);

  // Make sure the render engine is available.
  if (rendering::RenderEngine::Instance()->GetRenderPathType() ==
      rendering::RenderEngine::NONE)
  {
    gzerr << "No rendering engine, unable to run heights test\n";
    return;
  }

  // Make sure we can get a valid pointer to the scene.
  rendering::ScenePtr scene = GetScene();
  ASSERT_TRUE(scene);

  rendering::Heightmap *heightmap = NULL;

  // Wait for the heightmap to get loaded by the scene.
  {
    int i = 0;
    while (i < 20 && (heightmap = scene->GetHeightmap()) == NULL)
    {
      common::Time::MSleep(100);
      i++;
    }

    if (i >= 20)
      gzthrow("Unable to get heightmap");
  }

  physics::ModelPtr model = GetModel("heightmap");
  EXPECT_TRUE(model);

  physics::CollisionPtr collision =
    model->GetLink("link")->GetCollision("collision");

  physics::HeightmapShapePtr shape =
    boost::dynamic_pointer_cast<physics::HeightmapShape>(collision->GetShape());

  EXPECT_TRUE(shape);
  EXPECT_TRUE(shape->HasType(physics::Base::HEIGHTMAP_SHAPE));

  EXPECT_TRUE(shape->GetPos() == ignition::math::Vector3d(0, 0, 0));
  EXPECT_TRUE(shape->GetSize() == ignition::math::Vector3d(129, 129, 10));

  std::vector<float> physicsTest;
  std::vector<float> renderTest;

  float x, y;

  for (y = 0; y < shape->GetSize().y && y < .3; y += 0.2)
  {
    for (x = 0; x < shape->GetSize().x && x < 1; x += 0.2)
    {
      // Compute the proper physics test point.
      int xi = rint(x);
      if (xi >= shape->GetSize().x)
        xi = shape->GetSize().x - 1.0;

      int yi = rint(y);
      if (yi >= shape->GetSize().y)
        yi = shape->GetSize().y - 1.0;

      // Compute the proper render test point.
      double xd = xi - (shape->GetSize().x) * 0.5;
      double yd = (shape->GetSize().y) * 0.5 - yi;

      // The shape->GetHeight function requires a point relative to the
      // bottom left of the heightmap image
      physicsTest.push_back(shape->GetHeight(xi, yi));

      // The render test requires a point relative to the center of the
      // heightmap.
      renderTest.push_back(heightmap->GetHeight(xd, yd));

      // Debug output
      if (fabs(physicsTest.back() - renderTest.back()) >= 0.04)
      {
        std::cout << "Render XY[" << xd << " " << yd << "] Physics XY[" << xi
          << " " << yi << "] R[" << renderTest.back() << "] P["
          << physicsTest.back() << "] D["
          << fabs(renderTest.back() - physicsTest.back()) << "]\n";
      }

      // Test to see if the physics height is equal to the render engine
      // height.
      EXPECT_NEAR(physicsTest.back(), renderTest.back(), 0.04);
    }
  }

  float diffMax, diffSum, diffAvg;
  FloatCompare(&physicsTest[0], &renderTest[0], physicsTest.size(),
      diffMax, diffSum, diffAvg);

  EXPECT_LT(diffMax, 0.04);
  EXPECT_LT(diffSum, 0.2);
  EXPECT_LT(diffAvg, 0.02);

  printf("Max[%f] Sum[%f] Avg[%f]\n", diffMax, diffSum, diffAvg);

  // This will print the heights
  // printf("static float __heights[] = {");
  // unsigned int i=0;
  // for (y = 0; y < shape->GetVertexCount().y; ++y)
  // {
  //   for (x = 0; x < shape->GetVertexCount().x; ++x)
  //   {
  //     if (y == shape->GetVertexCount().y && x == shape->GetVertexCount().x)
  //       break;

  //     if (i % 7 == 0)
  //       printf("\n");
  //     else
  //       printf(" ");
  //     printf("%f,", shape->GetHeight(x, y));
  //     i++;
  //   }
  // }
  // printf(" %f};\nstatic float *heights = __heights;\n",
  // shape->GetHeight(x,y));
}
*/

/////////////////////////////////////////////////
TEST_F(HeightmapTest, NotSquareImage)
{
  NotSquareImage();
}

/////////////////////////////////////////////////
TEST_F(HeightmapTest, InvalidSizeImage)
{
  InvalidSizeImage();
}

/////////////////////////////////////////////////
TEST_P(HeightmapTest, PhysicsLoad)
{
  PhysicsLoad(GetParam());
}

/////////////////////////////////////////////////
TEST_P(HeightmapTest, WhiteAlpha)
{
  WhiteAlpha(GetParam());
}

/////////////////////////////////////////////////
TEST_P(HeightmapTest, WhiteNoAlpha)
{
  WhiteNoAlpha(GetParam());
}

/////////////////////////////////////////////////
TEST_P(HeightmapTest, Volume)
{
  Volume(GetParam());
}

/////////////////////////////////////////////////
TEST_P(HeightmapTest, LoadDEM)
{
  LoadDEM(GetParam());
}

/////////////////////////////////////////////////
//
// Disabled: segfaults ocassionally
// See https://bitbucket.org/osrf/gazebo/issue/521 for details

/*
TEST_P(HeightmapTest, Heights)
{
  Heights(GetParam());
}
*/

INSTANTIATE_TEST_CASE_P(PhysicsEngines, HeightmapTest, PHYSICS_ENGINE_VALUES);

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
