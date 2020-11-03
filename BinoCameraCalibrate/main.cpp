#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>



using namespace cv;
using namespace std;


bool image_capture = false;//����ȫ�ֱ�������������ɼ��궨ͼ��
//����¼���Ӧ��������������ɼ��궨ͼ��˫��������һ�βɼ�һ��ͼ��
void on_mouse(int event, int x, int y, int flags, void* a)
{
	if (event == EVENT_LBUTTONDBLCLK)
	{
		image_capture = true;
	}

}

void calobjectPoints(vector<Point3f>& obj, Size &boardSize, int squareSize);

int main(int argc, char* argv[])
{
	Size boardSize = Size(10, 8);  // �궨���̸���ڽǵ�ߴ磨��7x7��
	float squareSize = 20.0;       // �궨���Ϻڰ׸��ӵ�ʵ�ʱ߳���mm��
	int nFrames = 20;             // ���ڱ궨��ͼ����Ŀ
	string outputFileName;         // ����ļ�������
	bool showUndistorsed = true;
	//vector<string> imageList;
	vector<string> img_list_1;
	vector<string> img_list_2;
	Size imageSize;
	Size imageSize1;
	Size imageSize2;

	int calib_pattern = 0;
	cout << "����һ��˫Ŀ�Ӿ�����" << endl;
	cout << "���ȣ���ѡ��������궨ģʽ��1��������ͷ�ɼ��궨ͼ�񣩻�2����ͼ�����л�ȡ�궨ͼ��" << endl;
	cin >> calib_pattern;

	/************************************************************************************/
	/*********************�����ʵʱ�ɼ��궨ͼ�񲢱��浽ָ���ļ���***********************/
	//�����롰1���������������������Ԥ�����ں��������ɼ��궨ͼ��
	//��Ҫ�ɼ�ͼ�����Ŀ�ɱ���nrFrames�������ɹ���nrFrames��ͼ��Ԥ�������Զ��ر�
	if (calib_pattern == 1)
	{
		//VideoCapture inputCapture;
		VideoCapture inputCapture1;
		VideoCapture inputCapture2;
		//inputCapture.open(1);
		inputCapture2.open(2);
		inputCapture1.open(1);

		//if(!inputCapture.isOpened()==true) return -1;
		if (!inputCapture1.isOpened() == true) return -1;
		if (!inputCapture2.isOpened() == true) return -1;

		//inputCapture.set(CV_CAP_PROP_FRAME_WIDTH, 640);  
	   // inputCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 480); 

		inputCapture1.set(CAP_PROP_FRAME_WIDTH, 960);//�������ɼ�ͼ��ķֱ��ʴ�С  
		inputCapture1.set(CAP_PROP_FRAME_HEIGHT, 720);
		inputCapture2.set(CAP_PROP_FRAME_WIDTH, 960);
		inputCapture2.set(CAP_PROP_FRAME_HEIGHT, 720);

		/*
		inputCapture1.set(CV_CAP_PROP_FRAME_WIDTH, 640);//�������ɼ�ͼ��ķֱ��ʴ�С
		inputCapture1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		inputCapture2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		inputCapture2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		*/
		//namedWindow("�궨ͼ��ɼ�Ԥ������",CV_WINDOW_AUTOSIZE);
		namedWindow("����������1���궨ͼ��ɼ�Ԥ������", WINDOW_AUTOSIZE);
		namedWindow("����������2���궨ͼ��ɼ�Ԥ������", WINDOW_AUTOSIZE);

		//��������¼���������������ɼ��궨ͼ����ָ������˫��������һ�βɼ�һ��ͼ��
		//cvSetMouseCallback("�궨ͼ��ɼ�Ԥ������",on_mouse,NULL);
		setMouseCallback("����������1���궨ͼ��ɼ�Ԥ������", on_mouse, NULL);

		//Mat src_image;
		Mat src_img_1;
		Mat src_img_2;
		int capture_count = 0;

		while (1)
		{
			//inputCapture>>src_image;
			inputCapture1 >> src_img_1;
			inputCapture2 >> src_img_2;

			//imshow("�궨ͼ��ɼ�Ԥ������",src_image);
			imshow("����������1���궨ͼ��ɼ�Ԥ������", src_img_1);
			imshow("����������2���궨ͼ��ɼ�Ԥ������", src_img_2);
			waitKey(35);
			if (image_capture == true && capture_count < nFrames)
			{
				//Mat cap;
				Mat cap1;
				Mat cap2;
				//inputCapture>>cap;
				inputCapture1 >> cap1;
				inputCapture2 >> cap2;
				char address[100];

				//ƴ�ձ궨ͼ����·��������
				//sprintf(address,"Calibration_Image_Camera\\Image%d%s",capture_count+1,".jpg");
				//imwrite(address,cap);
				sprintf(address, "Calibration_Image_Camera\\Image_l%d%s", capture_count + 1, ".jpg");
				imwrite(address, cap1);
				sprintf(address, "Calibration_Image_Camera\\Image_r%d%s", capture_count + 1, ".jpg");
				imwrite(address, cap2);
				capture_count++;
				image_capture = false;
			}
			else if (capture_count >= nFrames)
			{
				cout << "�궨ͼ��ɼ���ϣ����ɼ���" << capture_count << "�ű궨ͼ��" << endl;
				//destroyWindow("�궨ͼ��ɼ�Ԥ������");
				destroyWindow("����������1���궨ͼ��ɼ�Ԥ������");
				destroyWindow("����������2���궨ͼ��ɼ�Ԥ������");
				image_capture = false;
				break;
			}
		}
	}

	/***********************************************************************************/
	/*******************����ű궨ͼ���·�����뵽imageList��1/2��������****************/
	if (calib_pattern == 1 || calib_pattern == 2)
	{
		char name[100];
		for (int i = 1; i <= nFrames; i++)
		{
			//sprintf(name,"Calibration_Image_Camera/Image%d%s",i,".jpg");
			//imageList.push_back(name);
			sprintf(name, "Calibration_Image_Camera1/Image_l%d%s", i, ".jpg");
			img_list_1.push_back(name);
			sprintf(name, "Calibration_Image_Camera1/Image_r%d%s", i, ".jpg");
			img_list_2.push_back(name);
		}
	}
	//cout<<"ImageList.size:"<<imageList.size()<<endl;
	cout << "Image list 1 size:" << img_list_1.size() << endl;
	cout << "Image list 2 size:" << img_list_2.size() << endl;

	/************************************************************************************/
	/****************�궨ǰ��׼����������ȡObject Points��Image Points*********************/
	//1 ���ȸ���square size(���̸��ʵ�ʱ߳�������20mm)��board size(���̸�Ľǵ���������5x5)��
	// ����forѭ���õ��ǵ����������Object Points��Z�������Ϊ0��
	//2 ����forѭ����findChessboardCorners()�����õ���ǵ�������������objectPoints��Ӧ��ͼ��������������imagePoints

	//vector<vector<Point2f> > imagePoints;//����ͼ���ҵ��Ľǵ�ļ���
	//vector<vector<Point3f> > objectPoints(1);
	vector<vector<Point2f>> img_pts_1;
	vector<vector<Point3f>> obj_pts_1(1);//��ʱ�ȶ���һά��objectPoints1����ȷ����imagePoints��ά��֮���ٽ�������
	vector<vector<Point2f>> img_pts_2;
	vector<vector<Point3f>> obj_pts_2(1);
	/*
	//����calobjectPoints()�������ڵõ����̸�ǵ���������꼯
	calobjectPoints(objectPoints[0], boardSize,squareSize);

	//��ͨ���ı����showChessboardCorner��ֵ��ȷ���Ƿ�չʾ��ȡ�ǵ���ͼ��
	bool displayCorners = false;
	for(int i=0;i<imageList.size();i++)
	{
		Mat src=imread(imageList[i],1);
		//imshow("��ʾ",src);
		imageSize = src.size();
		vector<Point2f> pointBuf;//ĳһ��ͼ���ҵ��Ľǵ�
		bool found=findChessboardCorners( src, boardSize, pointBuf, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);//��һ���ȽϷ�ʱ
		if(found)
		{
			Mat grayimage;
			cvtColor(src, grayimage, COLOR_BGR2GRAY);
			cornerSubPix( grayimage, pointBuf, Size(11,11),Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
			imagePoints.push_back(pointBuf);
			if(displayCorners)
			{
				Mat MidImage=src.clone();
				drawChessboardCorners( MidImage, boardSize, Mat(pointBuf), found );
				imshow("�ǵ��ȡ���",MidImage);
				waitKey(300);
			}
			destroyWindow("�ǵ��ȡ���");
		}
	}
	objectPoints.resize(imagePoints.size(),objectPoints[0]);
	*/

	calobjectPoints(obj_pts_1[0], boardSize, (int)squareSize);

	//��ͨ���ı����displayCorners1��ֵ��ȷ���Ƿ�չʾ��ȡ�ǵ���ͼ��
	bool displayCorners1 = false;
	for (int i = 0; i < img_list_1.size(); i++)
	{
		Mat src1 = imread(img_list_1[i], 1);
		imageSize = src1.size();
		vector<Point2f> pointBuf1;
		//ʹ�ò�ͬ��FLAG�������궨�������С
		bool found1 = findChessboardCorners(src1, boardSize, pointBuf1);
		//bool found1=findChessboardCorners( src1, boardSize, pointBuf1, CALIB_CB_ADAPTIVE_THRESH);
		//bool found1=findChessboardCorners( src1, boardSize, pointBuf1, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);//��һ���ȽϷ�ʱ
		if (found1)
		{
			Mat grayimage1;
			cvtColor(src1, grayimage1, COLOR_BGR2GRAY);
			cornerSubPix(grayimage1, pointBuf1, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
			img_pts_1.push_back(pointBuf1);

			if (displayCorners1)
			{
				Mat MidImage1 = src1.clone();
				drawChessboardCorners(MidImage1, boardSize, Mat(pointBuf1), found1);
				imshow("������ǵ��ȡ���", MidImage1);
				waitKey(300);
			}

			destroyWindow("������ǵ��ȡ���");
		}
	}
	//����resize()������objectPoints������������
	obj_pts_1.resize(img_pts_1.size(), obj_pts_1[0]);

	calobjectPoints(obj_pts_2[0], boardSize, (int)squareSize);
	bool displayCorners2 = false;
	for (int i = 0; i < img_list_2.size(); i++)
	{
		Mat src2 = imread(img_list_2[i], 1);
		//imageSize= src2.size();
		vector<Point2f> pointBuf2;

		//ʹ�ò�ͬ��FLAG�������궨�������С
		bool found2 = findChessboardCorners(src2, boardSize, pointBuf2);
		//bool found2=findChessboardCorners( src2, boardSize, pointBuf2, CALIB_CB_ADAPTIVE_THRESH);
		//bool found2=findChessboardCorners( src2, boardSize, pointBuf2, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);//��һ���ȽϷ�ʱ
		if (found2)
		{
			Mat grayimage2;
			cvtColor(src2, grayimage2, COLOR_BGR2GRAY);
			cornerSubPix(grayimage2, pointBuf2, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
			img_pts_2.push_back(pointBuf2);
			if (displayCorners2)
			{
				Mat MidImage2 = src2.clone();
				drawChessboardCorners(MidImage2, boardSize, Mat(pointBuf2), found2);
				imshow("������ǵ��ȡ���", MidImage2);
				waitKey(300);
			}
			destroyWindow("������ǵ��ȡ���");
		}
	}
	obj_pts_2.resize(img_pts_2.size(), obj_pts_2[0]);

	/**************************************************************************************/
	/********************************��������궨******************************************/
	//ͨ��calibrateCamera()������������궨
	//��ҪΪ�˵õ�������ڲξ���cameraMatrix������ϵ������distCoeffs
	//�������ͨ���������ص���ͶӰ����С��������궨�ľ������
	//����õ��������ξ�����Ҫ

	//Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	//Mat distCoeffs = Mat::zeros(8, 1, CV_64F);//����ϵ����˳����[k1,k2,p1,p2,k3,(k4,k5,k6)]
	Mat cameraMatrix1 = Mat::eye(3, 3, CV_64F);
	Mat distCoeffs1 = Mat::zeros(5, 1, CV_64F);
	Mat cameraMatrix2 = Mat::eye(3, 3, CV_64F);
	Mat distCoeffs2 = Mat::zeros(5, 1, CV_64F);
	//vector<Mat> rvecs, tvecs;
	vector<Mat> rvecs1, tvecs1;
	vector<Mat> rvecs2, tvecs2;
	/*
	double re_project_err=calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,0);
	//checkRange()����---���ڼ������е�ÿһ��Ԫ���Ƿ���ָ����һ����ֵ����֮��
	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
	if(ok)
	{
		cout<<"����궨�ɹ���"<<endl;
		cout<<"����궨����ͶӰ��"<<re_project_err<<endl;
		cout<<"����ڲξ���"<<endl<<cameraMatrix<<endl;
		cout<<"�������ϵ������"<<endl<<distCoeffs<<endl;
		if(showUndistorsed==true)
		{
			for(int i=0;i<imageList.size();i++)
			{
				Mat temp=imread(imageList1[i],1);;
				//����undistort()�����õ��������������ͼ��
				Mat undistort_view;
				undistort(temp, undistort_view, cameraMatrix, distCoeffs);
				imshow("ԭ����ͼ��",temp);
				imshow("�������ͼ��",undistort_view);
				waitKey(300);
			}
			destroyWindow("ԭ����ͼ��");
			destroyWindow("�������ͼ��");
		}
	}
	*/
	/*
	calibrateCamera()
	������� objectPoints  �ǵ��ʵ����������
			 imagePoints   �ǵ��ͼ������
			 imageSize     ͼ��Ĵ�С
	�������
			 cameraMatrix  ������ڲξ���
			 distCoeffs    ����Ļ������
			 rvecs         ��תʸ��(�����)
			 tvecs         ƽ��ʸ��(�������
	*/

	//����ϵ����˳����[k1,k2,p1,p2,k3,(k4,k5,k6)]
	//������ǲ���ҪK3���ڳ�ʼ��K3ΪO֮�󣬿���ʹ�ñ�־CV_CALIB_FIX_K3���������궨��������ı�K3��ֵ
	//һ��أ�K3Ӧ����Ϊ0������ʹ�����۾�ͷ���ο���learning opencv����ʮһ�£�
	//���ص�distCoeffs1�����ĳ����ɱ�־λflag��������flag����ΪCV_CALIB_RATIONAL_MODELʱ�������л��������8����
	//�����ó�����flagʱ������5ά�Ļ���ϵ������[k1,k2,p1,p2,k3]
	double re_project_err1 = calibrateCamera(obj_pts_1, img_pts_1, imageSize, cameraMatrix1, distCoeffs1, rvecs1, tvecs1, CALIB_FIX_K3);
	//checkRange()����---���ڼ������е�ÿһ��Ԫ�ص���Ч��
	bool ok1 = checkRange(cameraMatrix1) && checkRange(distCoeffs1);
	if (ok1)
	{
		cout << "������궨�ɹ���" << endl;
		cout << "������궨����ͶӰ��" << re_project_err1 << endl;
		//cout<<"������ڲξ���"<<endl<<cameraMatrix1<<endl;
		//cout<<"���������ϵ������"<<endl<<distCoeffs1<<endl;
	}
	double re_project_err2 = calibrateCamera(obj_pts_2, img_pts_2, imageSize, cameraMatrix2, distCoeffs2, rvecs2, tvecs2, CALIB_FIX_K3);
	bool ok2 = checkRange(cameraMatrix2) && checkRange(distCoeffs2);
	if (ok2)
	{
		cout << "������궨�ɹ���" << endl;
		cout << "������궨����ͶӰ��" << re_project_err2 << endl;
		//cout<<"������ڲξ���"<<endl<<cameraMatrix2<<endl;
		//cout<<"���������ϵ������"<<endl<<distCoeffs2<<endl;
	}

	/**************************************************************************************/
	/********************************����궨******************************************/
	//����stereoCalibrate()������������궨���õ�4������R ��תʸ�� Tƽ��ʸ�� E�������� F��������
	//ͬʱ���Եõ�����������ڲξ���ͻ���ϵ������cameraMatrix1��distCoeffs1��cameraMatrix2��distCoeffs2
	//Ҳ����˵��ʵ���Բ������ȵ�����ÿ��������б궨��
	//stereoCalibrate()�����ڲ�Ӧ���ǵ�����calibrateCamera()������
	//����Ҳ�����ȵ���calibrateCamera()�����ȶ�ÿ��������б궨���õ��ڲξ���ͻ���ϵ���ĳ�ʼֵ
	//Ȼ������궨ʱstereoCalibrate()��������ڲξ���ͻ���ϵ�������Ż�����ʱӦ����flag��CALIB_USE_INTRINSIC_GUESS
	//���û�����ȵ��ù�calibrateCamera()�������벻Ҫʹ��flag:CALIB_USE_INTRINSIC_GUESS����õ�����ֵĽ��
	//���֮ǰ�궨��������ڲξ���ͻ�����������⣬����������궨ʱ����һ���Ż�����ʹ��CV_CALIB_FIX_INTRINSIC
	//���ݹٷ��ĵ����飬stereoCalibrate()��������Ĳ����ռ��ά���ܸߣ�һ���Եõ��ܶ�����
	//���ܻᵼ��ĳЩ�����ɢ���������ֵ��ƫ����ȷ����������ǰʹ����calibrateCamera()������ÿ��������й��궨
	//�����ѡ��CV_CALIB_FIX_INTRINSICӦ�õ�stereoCalibrate()�����У������ܼ��ټ���Ĳ���
	//��ֹ����ĳЩ�����ɢ���������ֵ
	//CV_CALIB_FIX_INTRINSIC��������Ƿ�ʹ�û����������һ��Ȩ��
	Mat R = Mat::eye(3, 3, CV_64F);
	Mat T = Mat::zeros(3, 1, CV_64F);
	Mat E = Mat::zeros(3, 3, CV_64F);
	Mat F = Mat::eye(3, 3, CV_64F); //R ��תʸ�� Tƽ��ʸ�� E�������� F��������  
	double rms = stereoCalibrate(obj_pts_1, img_pts_1, img_pts_2,
		cameraMatrix1, distCoeffs1,
		cameraMatrix2, distCoeffs2,
		imageSize, R, T, E, F,
		CALIB_FIX_INTRINSIC,
		TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 100, 1e-5));
	/*double rms = stereoCalibrate(objectPoints1, imagePoints1, imagePoints2,
		cameraMatrix1, distCoeffs1,
		cameraMatrix2, distCoeffs2,
		imageSize, R, T, E, F,
		CALIB_USE_INTRINSIC_GUESS|CV_CALIB_FIX_K3,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 1e-6)); */
	cout << "Stereo Calibration done with RMS error = " << rms << endl;
	cout << "������ڲξ���" << endl << cameraMatrix1 << endl;
	cout << "���������ϵ������" << endl << distCoeffs1 << endl;
	cout << "������ڲξ���" << endl << cameraMatrix2 << endl;
	cout << "���������ϵ������" << endl << distCoeffs2 << endl;
	cout << "R:" << endl << R << endl;
	cout << "T:" << endl << T << endl;

	/**************************************************************************************/
	/********************************����궨���******************************************/
	//����.xml�ļ�ʱ��Ҫע��2�����⣺
	//1 ��Ҫ�����Mat�ͱ�������ʱ����Ҫ��ʼ������������������
	//2 ����ʱ�����ı�ʶ�������в��ܳ��֡�.����
	const string filename = "./Calibration_Result.xml";
	//string filename="Calibration_Result.xml";
	FileStorage fs(filename, FileStorage::WRITE);
	if (fs.isOpened())
	{
		fs << "width" << imageSize.width;
		fs << "height" << imageSize.height;
		fs << "board_width" << boardSize.width;
		fs << "board_height" << boardSize.height;
		fs << "nrFrames" << nFrames;
		fs << "cameraMatrix1" << cameraMatrix1;
		fs << "distCoeffs1" << distCoeffs1;
		fs << "cameraMatrix2" << cameraMatrix2;
		fs << "distCoeffs2" << distCoeffs2;
		fs << "R" << R;
		fs << "T" << T;
		fs << "E" << E;
		fs << "F" << F;
		fs.release();
		cout << "Calibration result has been saved successfully to \nF:\\Binocular_Stereo_Vision_Test\\Calibration_Result.xml" << endl;
	}
	else
	{
		cout << "Error: can not save the Calibration result!!!!!" << endl;
	}

	cout << "��������˳�����..." << endl;

	//while (1)
	//{
	//	int a = waitKey(10);
	//	if (char(a) == 27)
	//	{
	//		break;
	//	}
	//}

	waitKey(0);
	return 0;
}

/*����궨����ģ���ʵ����������*/
void calobjectPoints(vector<Point3f>& obj, Size &boardSize, int squareSize)
{
	for (int i = 0; i < boardSize.height; ++i)
	{
		for (int j = 0; j < boardSize.width; ++j)
		{
			obj.push_back(Point3f((float)(j*squareSize), (float)(i*squareSize), 0.f));
		}
	}
}