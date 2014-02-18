README file EE569 Homework#1

All files are successfully compiled under:

Linux...........ubuntu 12.04
Platform........eclipse CDT indigo [version 6.0]
Text editor.....Notepad++

Please run files under this environment.

Filenames:
they are listed in the order of homework problems, please run them in the following order
	convert2Grey.cpp	| Convert color to grayscale
						| 	arguments: [program_name] [input_image.raw] [output_image.raw] [BytesPerPixel = 1] [SizeW=256] [SizeH=256]
						| 	ouput: input_imageAVG.raw , input_imageLUM.raw
	
	contrastLS.cpp		| Linear contrast enhancement
						|	this program assume image size is 400x300
						|	arguments: [program_name] [input_image.raw] [output_image.raw]
						|	output: two linear scaled image using clipping and absolute value
						
	contrastSR.cpp		| Square root scaling
						| 	arguments: [program_name] [input_image.raw] [output_image.raw]
						| 	ouput: square root scaled image
						
	contrastHIST.cpp	| histogram equalization
						|	arguments: [program_name] [input_image.raw] [output_image.raw]
						|	output: cdf and digital method image along with CDF and PDF of the input image
						|	CDF and PDF processing part can be used to generate cdf and pdf for other images.
						
	contasrtLOCAL.cpp	| Local contrast enhancement
						| 	arguments: [program_name] [input_image.raw] [output_image.raw]
						| 	ouput: center weighted contrast enhancement
	
	denoise.cpp			| denoising
						|	arguments: [program_name] [input_image.raw] [output_image.raw]
 						| 	need manually change strings: method1, method2 and method3
						|	to achieve cascaded filter combinations, the output name would change automatically
						
	histogram.cpp		| a stand alone program for histogram generation
						|	argument: none
						|	ouput: text files of cdp and pdf
						|	!!!To run the program, need change line:37,81 and 83 for input and output name!!!
						
	MSE.cpp				| MSE, PSNR and SSIM error generation
						|	argument: [input_image1.raw] [imput_image2.raw]
						|	output: absolute error map and SSIM index map
						|	and also will update text file for index values,
						|	if text file not found, program will create a new one.
	
	edgeDoG.cpp			| difference of Gaussian for edge detection
						|	arguments: [program_name] [input_image.raw] [output_image.raw]
						|	change 	line:166 for threshold selection
						|			line:177 for output name
						
	edgeLoG.cpp			| lapacian of gaussian
						|	arguments: [program_name] [input_image.raw] [output_image.raw]
						|	change	line156 and line159 for threshold
						
	edgeVariance.cpp	| variance map edge detection
						|	arguments: [program_name] [input_image.raw] [output_image.raw]
						
	edgeThin.cpp		| edge thinning
						|	arguments: [program_name] [raw_x_gradient_map.raw] [raw_y_gradient_map.raw] [edge_map.raw]
						|	output: edge_map_thin.raw
	
	
	