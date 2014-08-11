#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

#include <pcl/point_types.h>
#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>
#include <pcl/io/vtk_io.h>
#include <pcl/io/vtk_lib_io.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/common/transforms.h>
#include <pcl/visualization/image_viewer.h>

void generateMesh(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud, pcl::PolygonMesh::Ptr triangles, int maxNN) {

	// Normal estimation*
	  pcl::NormalEstimation<pcl::PointXYZRGB, pcl::Normal> n;
	  pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
	  pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZRGB>);
	  tree->setInputCloud (cloud);
	  n.setInputCloud (cloud);
	  n.setSearchMethod (tree);
	  n.setKSearch (20);
	  n.compute (*normals);
	  //* normals should not contain the point normals + surface curvatures

	  // Concatenate the XYZ and normal fields*
	  pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_with_normals (new pcl::PointCloud<pcl::PointXYZRGBNormal>);
	  pcl::concatenateFields (*cloud, *normals, *cloud_with_normals);
	  //* cloud_with_normals = cloud + normals

	  std::cout << "Point A\n";

	  // Create search tree*
	  pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree2 (new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
	  tree2->setInputCloud (cloud_with_normals);

	  std::cout << "Point B\n";

	  // Initialize objects
	  pcl::GreedyProjectionTriangulation<pcl::PointXYZRGBNormal> gp3;
	  //pcl::PolygonMesh triangles;

	  std::cout << "Point C\n";

	  // Set typical values for the parameters
	  gp3.setMu (2.5);
	  gp3.setSearchRadius(0.05);
	  gp3.setMaximumNearestNeighbors (maxNN);
	  gp3.setMaximumSurfaceAngle(M_PI/4); // 45 degrees
	  gp3.setMinimumAngle(M_PI/18); // 10 degrees
	  gp3.setMaximumAngle(2*M_PI/3); // 120 degrees
	  gp3.setNormalConsistency(true);


	  // Get result
	  gp3.setInputCloud (cloud_with_normals);
	  gp3.setSearchMethod (tree2);
	  std::cout << "Point D: cloudnormals: " << cloud_with_normals->size() << std::endl;
	  std::cout << "num of triangles1: " << triangles->polygons.size() << std::endl;
	  std::cout << "search radius: " << gp3.getSearchRadius() << std::endl;;

	  gp3.reconstruct(*triangles);

	  std::cout << "Point E\n";

	  // Additional vertex information
	  std::vector<int> parts = gp3.getPartIDs();
	  std::vector<int> states = gp3.getPointStates();

	  std::cout << "num of triangles: " << triangles->polygons.size() << std::endl;
}

pcl::PointCloud<pcl::PointXYZRGB>::Ptr downsampleCloud(pcl::PointCloud<pcl::PointXYZRGB>::Ptr inputCloud, float dimension) {
	std::cout << "Downsampling cloud - initial size: " << inputCloud->points.size() << std::endl;

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr outputCloud(new pcl::PointCloud<pcl::PointXYZRGB>);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr returnCloud(new pcl::PointCloud<pcl::PointXYZRGB>);

	pcl::StatisticalOutlierRemoval<pcl::PointXYZRGB> sor;
	sor.setInputCloud(inputCloud);
	sor.setMeanK(50);
	sor.setStddevMulThresh(1.0);
	sor.filter(*outputCloud);

	  std::cout << "SOR completed - final size: " << outputCloud->points.size() << std::endl;

	// Create the filtering object
	  pcl::VoxelGrid<pcl::PointXYZRGB> vgrid;
	  vgrid.setInputCloud(outputCloud);
	  vgrid.setLeafSize(dimension, dimension, dimension);
	  vgrid.filter(*returnCloud);

	  std::cout << "Downsample completed - final size: " << returnCloud->points.size() << std::endl;

	  return returnCloud;
}

int main(int argc, char* argv[]) {
	std::time_t starttime;
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr inputcloud(new pcl::PointCloud<pcl::PointXYZRGB>);
	pcl::PolygonMeshPtr totalTriangles = pcl::PolygonMeshPtr(new pcl::PolygonMesh);

	if (argc < 3) {
		std::cout << "This program requires 2 arguments. The first is the input ply file and the second is the output ply file.\nDo not use ~ for /home/user.\n";
		exit(-1);
	}

	std::string inputfilename = std::string(argv[1]);
	std::cout << "Input Filename: " << inputfilename << std::endl;

	std::string outputfilename = std::string(argv[2]);
	std::cout << "Output Filename: " << outputfilename << std::endl;


	//load ply file
	starttime = std::time(NULL);
	pcl::io::loadPLYFile(inputfilename,*inputcloud);
	std::cout << "Elapsed time for loadPLYFile(): " << std::difftime(std::time(NULL), starttime) << " s.\n";
	std::cout << "Points loaded, size: " << inputcloud->size() << std::endl;
	std::cout << std::endl;

	//downsample and filter
	starttime = std::time(NULL);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr totalSampleSmall = downsampleCloud(inputcloud, 0.001);
	std::cout << "Elapsed time for downsampleCloud(): " << std::difftime(std::time(NULL), starttime) << " s.\n";
	std::cout << "Downsampled, new size: " << inputcloud->size() << std::endl;
	std::cout << std::endl;

	//generate mesh
	starttime = std::time(NULL);
	generateMesh(totalSampleSmall, totalTriangles, 500);
	std::cout << "Elapsed time for generateMesh(): " << std::difftime(std::time(NULL), starttime) << " s.\n";
	std::cout << "Mesh Gen, poly size: " << totalTriangles->polygons.size() << std::endl;
	std::cout << std::endl;

	//save new ply file
	starttime = std::time(NULL);
	pcl::io::savePLYFile(outputfilename, *totalTriangles,6);
	std::cout << "Elapsed time for savePLYFile(): " << std::difftime(std::time(NULL), starttime) << " s.\n";
}
