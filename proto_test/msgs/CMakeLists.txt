# 包含Protobuf包
find_package(Protobuf REQUIRED)
#find_package(sensor_msgs REQUIRED)
# 查找消息的安装位置
set(PROTOBUF_IMPORT_DIRS)
foreach(ITR ${GAZEBO_INCLUDE_DIRS})
    if(ITR MATCHES ".*gazebo-[0-11.]+$")
        set(PROTOBUF_IMPORT_DIRS "${ITR}/gazebo/msgs/proto")
    endif()
endforeach()

# 创建列表msgs，该列表使用vector2d.proto消息，并依赖time.proto和header.proto
set(msgs
        attackGPS.proto
)
set(sensor_msgs
        attackGPS.proto
        )
PROTOBUF_GENERATE_CPP(SEN_PROTO_SRCS SEN_PROTO_HDRS ${sensor_msgs})
add_library(sensor_msgs SHARED ${SEN_PROTO_SRCS})
# 构建必要的C++ 源文件
#PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${msgs})
#PROTOBUF_GENERATE_CPP(SEN_PROTO_SRCS SEN_PROTO_HDRS ${sensor_msgs})
#add_library(sensor_msgs SHARED ${SEN_PROTO_SRCS})
# 链接
#add_library(collision_map_creator_msgs SHARED ${PROTO_SRCS})
#add_library(sensor_msgs SHARED ${SEN_PROTO_SRCS})
#target_link_libraries(collision_map_creator_msgs ${PROTOBUF_LIBRARY})
