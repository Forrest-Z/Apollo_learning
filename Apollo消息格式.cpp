 人工发送prediction::PredictionObstacles消息
为提高消息处理的实时性和灵活性，Apollo 3.5的Planning模块不再基于定时器触发更新，而是基于三个输入消息的改变而动态更新，这三个输入消息分别为：prediction::PredictionObstacles、canbus::Chassis、localization::LocalizationEstimate。也就是说，只有上述三个消息同时存在时，Planning模块的消息处理函数PlanningComponent::Proc()才会被调用，而具体的某一类规划算法（例如OnLanePlanning）才会真正工作。

若某条消息因为特殊原因不能及时发送，解决办法就是人工生成假消息。例如，若不能收到prediction::PredictionObstacles消息，则可在在Docker内部通过如下命令生成假prediction::PredictionObstacles消息：

cyber_launch start /apollo/modules/tools/prediction/fake_prediction/fake_prediction.launch
1
该假消息的具体生成代码见/apollo/modules/tools/prediction/fake_prediction，其他假消息的生成可参照该示例撰写。

3.3.2 人工发送perception::TrafficLightDetection消息
调试规划算法时，需要动态改变红绿灯的信号状态，可以通过如下命令人工发送perception::TrafficLightDetection消息来实现：

cyber_launch start /apollo/modules/tools/manual_traffic_light/manual_traffic_light.launch




5. 
protobuffer:

zy@zy:~/Desktop/proto$ protoc addressbook.proto --cpp_out=./


6. the pb.h and pb.cc files generated by bazel are localized in the path below: 

root@apollo5-apollo-ubuntu14-1:/apollo/bazel-out/local-dbg/genfiles/modules/localization/proto# 


7. IMU publish:
  http://git.51vr.local/51World/Cybertron/blob/ue4.22/Modules/BridgeApollo/CybertronBridgeApolloCyber/TaskCyberChassisImu.cpp
  mCyberImu.publish(angvelX, angvelY, angvelZ, linAccX, linAccY, linAccZ); # 70


8. http://git.51vr.local/51World/Cybertron/blob/ue4.22/Modules/BridgeApollo/CybertronBridgeApolloCyber/TaskCyberPointCloud.cpp 

    8.1  mCyberPointCloud.publish(
            frame, 
            mPointCloud.width(),
            mPointCloud.height(),
            mPointCloud.pointstep(),
            mPointCloud.data().data()

        );                                        //CyberWriterPointCloud::publish(int sequence, int width, int height, int step, const char *data)


  8.2  CyberWriterPointCloud mCyberPointCloud;   // CyberWriterPointCloud.hpp

cybertron::proto::sensor::DataPointCloud mPointCloud; // the mPointCloud which has been published  

In cyvertron the Lidar protobuf:
cybertron::proto::sensor::DataPointCloud  //  PointCloud.pb.h
 





 

  8.4 msg.toProtobuf(mPointCloud)



   8.5
   http://git.51vr.local/51World/Cybertron/blob/master/Modules/Unreal/UnrealNodeBase/CybertronUnrealNodeBase/UnrealSensorTask.cpp_out

   void UnrealSensorTask::UpdateLaserRadarOutput(const UnrealBridge::SensorHeader& sensorHeader, int sensorId, 

                                                  const std::vector<uint8_t>& pointCloudData, 

                                                  const UnrealBridge::GroundTruthData& groundTruthData)






9. ROS   sensor_msgs/PointCloud 

        以上是标准头信息的主要部分。seq是消息的顺序标识，不需要手动设置，发布节点在发布消息时，会自动累加。stamp 是消息中与数据相关联的时间戳，例如激光数据中，时间戳对应激光数据的采集时间点。frame_id 是消息中与数据相关联的参考系id，例如在在激光数据中，frame_id对应激光数据采集的参考系

        #Standard metadata for higher-level flow data types
#sequence ID: consecutively increasing ID 
uint32 seq
 
#Two-integer timestamp that is expressed as:
# * stamp.secs: seconds (stamp_secs) since epoch
# * stamp.nsecs: nanoseconds since stamp_secs
# time-handling sugar is provided by the client library
time stamp
 
#Frame this data is associated with
# 0: no frame
# 1: global frame
string frame_id





10. Apollo pointcloud.proto:

    syntax = "proto2";
    package apollo.drivers;

    import "modules/common/proto/header.proto";

    // 一帧点云
    message PointXYZIT {
      optional float x = 1 [default = nan];          //
      optional float y = 2 [default = nan];          //
      optional float z = 3 [default = nan];          //
      optional uint32 intensity = 4 [default = 0];  //激光反射强度（Intensity
      optional uint64 timestamp = 5 [default = 0];  // 每一个点的时间戳
    }

    message PointCloud {
      optional apollo.common.Header header = 1;   //
      optional string frame_id = 2;               //
      optional bool is_dense = 3;                 //
      repeated PointXYZIT point = 4;
      optional double measurement_time = 5;        //last_time_stamp_ = pointcloud->measurement_time();
      optional uint32 width = 6;                  //
      optional uint32 height = 7;                 //height与width是指点云数据的高和宽，一般无序点云的高为1，宽为点云中激光点的个数；结构化点云的高和宽均大于1。
    }




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
11. Cybretron PointCloud.proto


syntax = "proto3";

import "Common/Header.proto";
import "Sensor/ObstacleDetect.proto";

package cybertron.proto.sensor;

enum EPointCloudFieldName
{
  EPointCloudFieldName_XYZ = 0; // float x 3
  EPointCloudFieldName_Intensity = 1; // uint16
  EPointCloudFieldName_Ring = 2; // uint16
  EPointCloudFieldName_RGB = 3; // uint8 x 3
  EPointCloudFieldName_Segmentation = 4; // uint8
  EPointCloudFieldName_InstanceId = 5; // Int32
}

// refer to http://docs.ros.org/api/sensor_msgs/html/msg/PointField.html
enum EPointCloudFieldType
{
  EPointCloudFieldType_Unused = 0;
  EPointCloudFieldType_Int8 = 1;
  EPointCloudFieldType_UInt8 = 2;
  EPointCloudFieldType_Int16 = 3;
  EPointCloudFieldType_UInt16 = 4;
  EPointCloudFieldType_Int32 = 5;
  EPointCloudFieldType_UInt32 = 6;
  EPointCloudFieldType_Float = 7;
  EPointCloudFieldType_Double = 8;
}

message PointCloudField{
  EPointCloudFieldName name = 1;      //name是指点云包含的域的名称，如“x”、“y”、“z”、“intensity”、“ring”等
  uint32 offset = 2;                  //offset是指在data的每个数据中该域的偏移量
  EPointCloudFieldType datatype = 3;  //datatype是指以上1~8的数据类型
}

message DataPointCloud {
  common.CybertronHeader header = 1;
  // 2D structure of the point cloud.
  // For unordered point cloud, height is 1 and width is point count of the point cloud.
  uint32 width = 2;
  uint32 height = 3;
  // raw data fields definition.
  repeated PointCloudField fields = 4;
  // Length of a point in bytes
  uint32 pointStep = 5;    //pointstep表示每个点的字节长度，常见的为32
  // raw data, size should be width * height * pointStep
  bytes data = 6;     //data表示所有的点的数据，以字节存储。uint8[] data代表vector类型，有size、push_back、clear等操作 //  for(unsigned int i = 0; i < num_points; ++i){
                                                                                                             //           cloud.points[i].x = 0.1*i;
                                                                                                             //           cloud.points[i].y = 0.1*i;
                                                                                                             //          cloud.points[i].z = 5;
                                                                                                             //           cloud.channels[0].values[i] = 255;
                                                                                                             //  }                                                                                                          //        }


message PointCloudStamped{
  common.Header header = 1;
  DataPointCloud pointcloud = 2;
}

message PointCloudWithGroundTruth{
  DataPointCloud pointcloud = 1;
  GroundTruth ground_truth = 2;
}











5. 
protobuffer:

zy@zy:~/Desktop/proto$ protoc addressbook.proto --cpp_out=./


6. the pb.h and pb.cc files generated by bazel are localized in the path below: 

root@apollo5-apollo-ubuntu14-1:/apollo/bazel-out/local-dbg/genfiles/modules/localization/proto# 


7. IMU publish:
  http://git.51vr.local/51World/Cybertron/blob/ue4.22/Modules/BridgeApollo/CybertronBridgeApolloCyber/TaskCyberChassisImu.cpp
  mCyberImu.publish(angvelX, angvelY, angvelZ, linAccX, linAccY, linAccZ); # 70


8. http://git.51vr.local/51World/Cybertron/blob/ue4.22/Modules/BridgeApollo/CybertronBridgeApolloCyber/TaskCyberPointCloud.cpp 

    8.1  mCyberPointCloud.publish(
            frame, 
            mPointCloud.width(),
            mPointCloud.height(),
            mPointCloud.pointstep(),
            mPointCloud.data().data()

        );                                        //CyberWriterPointCloud::publish(int sequence, int width, int height, int step, const char *data)


  8.2  CyberWriterPointCloud mCyberPointCloud;   // CyberWriterPointCloud.hpp

cybertron::proto::sensor::DataPointCloud mPointCloud; // the mPointCloud which has been published  

In cyvertron the Lidar protobuf:
cybertron::proto::sensor::DataPointCloud  //  PointCloud.pb.h
 





 

  8.4 msg.toProtobuf(mPointCloud)



   8.5
   http://git.51vr.local/51World/Cybertron/blob/master/Modules/Unreal/UnrealNodeBase/CybertronUnrealNodeBase/UnrealSensorTask.cpp_out

   void UnrealSensorTask::UpdateLaserRadarOutput(const UnrealBridge::SensorHeader& sensorHeader, int sensorId, 

                                                  const std::vector<uint8_t>& pointCloudData, 

                                                  const UnrealBridge::GroundTruthData& groundTruthData)






9. ROS   sensor_msgs/PointCloud 

        以上是标准头信息的主要部分。seq是消息的顺序标识，不需要手动设置，发布节点在发布消息时，会自动累加。stamp 是消息中与数据相关联的时间戳，例如激光数据中，时间戳对应激光数据的采集时间点。frame_id 是消息中与数据相关联的参考系id，例如在在激光数据中，frame_id对应激光数据采集的参考系

        #Standard metadata for higher-level flow data types
#sequence ID: consecutively increasing ID 
uint32 seq
 
#Two-integer timestamp that is expressed as:
# * stamp.secs: seconds (stamp_secs) since epoch
# * stamp.nsecs: nanoseconds since stamp_secs
# time-handling sugar is provided by the client library
time stamp
 
#Frame this data is associated with
# 0: no frame
# 1: global frame
string frame_id





10. Apollo pointcloud.proto:

    syntax = "proto2";
    package apollo.drivers;

    import "modules/common/proto/header.proto";

    // 一帧点云
    message PointXYZIT {
      optional float x = 1 [default = nan];          //
      optional float y = 2 [default = nan];          //
      optional float z = 3 [default = nan];          //
      optional uint32 intensity = 4 [default = 0];  //激光反射强度（Intensity
      optional uint64 timestamp = 5 [default = 0];  // 每一个点的时间戳
    }

    message PointCloud {
      optional apollo.common.Header header = 1;   //
      optional string frame_id = 2;               //
      optional bool is_dense = 3;                 //
      repeated PointXYZIT point = 4;
      optional double measurement_time = 5;        //last_time_stamp_ = pointcloud->measurement_time();
      optional uint32 width = 6;                  //
      optional uint32 height = 7;                 //height与width是指点云数据的高和宽，一般无序点云的高为1，宽为点云中激光点的个数；结构化点云的高和宽均大于1。
    }




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
11. Cybretron PointCloud.proto


syntax = "proto3";

import "Common/Header.proto";
import "Sensor/ObstacleDetect.proto";

package cybertron.proto.sensor;

enum EPointCloudFieldName
{
  EPointCloudFieldName_XYZ = 0; // float x 3
  EPointCloudFieldName_Intensity = 1; // uint16
  EPointCloudFieldName_Ring = 2; // uint16
  EPointCloudFieldName_RGB = 3; // uint8 x 3
  EPointCloudFieldName_Segmentation = 4; // uint8
  EPointCloudFieldName_InstanceId = 5; // Int32
}

// refer to http://docs.ros.org/api/sensor_msgs/html/msg/PointField.html
enum EPointCloudFieldType
{
  EPointCloudFieldType_Unused = 0;
  EPointCloudFieldType_Int8 = 1;
  EPointCloudFieldType_UInt8 = 2;
  EPointCloudFieldType_Int16 = 3;
  EPointCloudFieldType_UInt16 = 4;
  EPointCloudFieldType_Int32 = 5;
  EPointCloudFieldType_UInt32 = 6;
  EPointCloudFieldType_Float = 7;
  EPointCloudFieldType_Double = 8;
}

message PointCloudField{
  EPointCloudFieldName name = 1;      //name是指点云包含的域的名称，如“x”、“y”、“z”、“intensity”、“ring”等
  uint32 offset = 2;                  //offset是指在data的每个数据中该域的偏移量
  EPointCloudFieldType datatype = 3;  //datatype是指以上1~8的数据类型
}

message DataPointCloud {
  common.CybertronHeader header = 1;
  // 2D structure of the point cloud.
  // For unordered point cloud, height is 1 and width is point count of the point cloud.
  uint32 width = 2;
  uint32 height = 3;
  // raw data fields definition.
  repeated PointCloudField fields = 4;
  // Length of a point in bytes
  uint32 pointStep = 5;    //pointstep表示每个点的字节长度，常见的为32
  // raw data, size should be width * height * pointStep
  bytes data = 6;     //data表示所有的点的数据，以字节存储。uint8[] data代表vector类型，有size、push_back、clear等操作 //  for(unsigned int i = 0; i < num_points; ++i){
                                                                                                             //           cloud.points[i].x = 0.1*i;
                                                                                                             //           cloud.points[i].y = 0.1*i;
                                                                                                             //          cloud.points[i].z = 5;
                                                                                                             //           cloud.channels[0].values[i] = 255;
                                                                                                             //  }                                                                                                          //        }


message PointCloudStamped{
  common.Header header = 1;
  DataPointCloud pointcloud = 2;
}

message PointCloudWithGroundTruth{
  DataPointCloud pointcloud = 1;
  GroundTruth ground_truth = 2;
}


