#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Eigen/Core>

#include <iostream>

#include <pcl/point_types.h>
#include <pcl/io/ply_io.h>
#include <pcl/io/vtk_lib_io.h>
 #include <pcl/registration/icp_nl.h>
#include <pcl/common/transforms.h>
#include <pcl/visualization/pcl_visualizer.h>



int main(int argc, char* argv[])
{
	int v1(0);
	int v2(0);
/*
	pcl::visualization::PCLVisualizer vis("Mesh Viewer");
	vis.createViewPort(0.0, 0.0, 0.5, 1.0, v1);
	vis.createViewPort(0.5, 0.0, 1.0, 1.0, v2);
	vis.setBackgroundColor(0.1, 0.01,1.0);
	vis.initCameraParameters();
	vis.setCameraPosition(-0.5,0,0, 0,0,1);
	vis.spinOnce(100);
*/
   pcl::PolygonMesh map_mesh;
   pcl::PolygonMesh ref_mesh;
   pcl::PolygonMesh final_mesh;

   pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr map_cloud(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
   pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr ref_cloud(new pcl::PointCloud<pcl::PointXYZRGBNormal>);

   pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr map_cloud_transformed(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
   pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr ref_cloud_transformed(new pcl::PointCloud<pcl::PointXYZRGBNormal>);
   pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr final_cloud(new pcl::PointCloud<pcl::PointXYZRGBNormal>);

   pcl::IterativeClosestPointNonLinear<pcl::PointXYZRGBNormal, pcl::PointXYZRGBNormal> icp;

   Eigen::Matrix4f map_transform;
   Eigen::Matrix4f ref_transform;

   ref_transform << 	0, -1, 0, 0,
					    -1, 0, 0, 0,
					    0, 0, -1, 0,
					    0, 0, 0, 1;

   map_transform << 	0, 0, 1, 0,
					    0, 1, 0, 0,
					    -1, 0, 0, 0,
					    0, 0, 0, 1;

   pcl::io::loadPolygonFilePLY("/home/tweddle/Dropbox/Public/newmesh/totalMeshBin.ply", map_mesh);
   std::cout <<  "Loaded map polygons: " << map_mesh.polygons.size() << std::endl;
   pcl::fromROSMsg(map_mesh.cloud,*map_cloud);
   std::cout << "Loaded map points: " << map_cloud->size() << std::endl;

   pcl::io::loadPolygonFilePLY("/home/tweddle/Dropbox/Public/newmesh/SPHERES_satellite_detailed_withEXPV2-v2.ply", ref_mesh);
   std::cout <<  "Loaded ref polygons: " << ref_mesh.polygons.size() << std::endl;
   pcl::fromROSMsg(ref_mesh.cloud,*ref_cloud);
   std::cout << "Loaded ref points: " << ref_cloud->size() << std::endl;

   pcl::transformPointCloud(*ref_cloud, *ref_cloud_transformed, ref_transform);
   pcl::toROSMsg(*ref_cloud_transformed, ref_mesh.cloud);
   pcl::transformPointCloud(*map_cloud, *map_cloud_transformed, map_transform);
   pcl::toROSMsg(*map_cloud_transformed, map_mesh.cloud);

   icp.setInputTarget(ref_cloud_transformed);
   icp.setInputCloud(map_cloud_transformed);
/*
   vis.addPointCloud(ref_cloud_transformed,"ref",v1);
   vis.addPointCloud(map_cloud_transformed,"map",v2);
   vis.spinOnce(100);
*/

//   pcl::io::savePolygonFilePLY("/home/tweddle/Dropbox/Public/newmesh/reference_transformed.ply", ref_mesh);
//   pcl::io::savePolygonFilePLY("/home/tweddle/Dropbox/Public/newmesh/map_transformed.ply", map_mesh);

   // Set the max correspondence distance to 5cm (e.g., correspondences with higher distances will be ignored)
   icp.setMaxCorrespondenceDistance (0.003);
   // Set the maximum number of iterations (criterion 1)
   icp.setMaximumIterations (300);
   // Set the transformation epsilon (criterion 2)
   icp.setTransformationEpsilon (1e-16);
   // Set the euclidean distance difference epsilon (criterion 3)
   icp.setEuclideanFitnessEpsilon (-1000);

   icp.setRANSACIterations(10000);
   icp.setRANSACOutlierRejectionThreshold(0.01);

   Eigen::Matrix4f initialGuess;
   initialGuess << 0,0,1,0,
				   -1,0,0,0,
				   0,-1,0,0,
				   0,0,0,1;

   std::cout << "Initial Guess: \n" << initialGuess <<std::endl;
   std::cout << "Max Correspondence Distance: " << icp.getMaxCorrespondenceDistance() <<std::endl;
   std::cout << "Max Iterations: " << icp.getMaximumIterations() <<std::endl;
   std::cout << "Transformation Epsilon: " << icp.getTransformationEpsilon() <<std::endl;
   std::cout << "Euclidean Fitness Epsilon: " << icp.getEuclideanFitnessEpsilon() <<std::endl;
   std::cout << "Ransac iter: " << icp.getRANSACIterations() <<std::endl;
   std::cout << "Ransac outlier thresh: " << icp.getRANSACOutlierRejectionThreshold() <<std::endl;


   // Perform the alignment
   //icp.align(final_cloud, initialGuess);
   icp.align(*final_cloud);
   //vis.addPointCloud(final_cloud,"final",v2);

   std::cout << "has converged:" << icp.hasConverged() << " score: " << icp.getFitnessScore() << std::endl;
   std::cout << "Final Transform: \n" <<icp.getFinalTransformation() << std::endl;

   final_mesh = map_mesh;
   pcl::toROSMsg(*final_cloud, final_mesh.cloud);
   pcl::io::savePolygonFilePLY("/home/tweddle/Dropbox/Public/newmesh/registered_map.ply", final_mesh);

/*
	for (int j = 0; j < 100; j++) {
		vis.spinOnce(100);
		usleep(50000);
	}
*/
   return 0;
}
