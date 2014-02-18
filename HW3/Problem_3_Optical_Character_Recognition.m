function Problem_3_Optical_Character_Recognition()
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EE 569 Homework Assignment #3 - Problem 2 Optical Character Recognition
% Name : Zhaoxuan Ma
% USC ID : 3564031656
% USC Email : zhaoxuam@usc.edu
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Initial parameter setting:
% Image size setting-------------------------------------------------------
% Sample image size information:
samplexlvl=200; sampleylvl=300; samplesize=samplexlvl*sampleylvl;
% Testing image size information:
testxlvl=200; testylvl=300; testsize=testxlvl*testylvl;
% License image size information:
licxlvl=200; licylvl=300; licsize=licxlvl*licylvl;
% Plate image size information:
platexlvl=1200; plateylvl=500; platesize=platexlvl*plateylvl;
% File path setting--------------------------------------------------------
% Sample image filepath:
samplepath=cell(1,10);
for i=1:10
samplepath{i}=['E:\Downloads\EE569\code\Problem 3\training\training' num2str(i) '.raw'];
end
% Testing image filepath:
testpath=cell(1,10);
for i=1:10
testpath{i}=['E:\Downloads\EE569\code\Problem 3\testing\testing' num2str(i) '.raw'];
end
% License image filepath:
licpath=cell(1,7);
for i=1:10
licpath{i}=['E:\Downloads\EE569\code\Problem 3\license\license' num2str(i) '.raw'];
end
% Plate image filepath
platepath=['F:\master program\EE 569\HW3 Materials\HW3_images\Problem 3\(d) bonus\plate.raw'];
% Save folder:
folder='Problem_3_Results';
mkdir(folder);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Functions:
% he : histogram equalization function
% rgbtohsi : change color image from RGB format into HSI format
% bondingbox : get the vertex position of a bondingbox for an object
% thinning : thinning processing for one image
% contdigit : count the digit expression of a window
% countbond : count the bond of a window
% bitquad : calculate the bit quad number
% separate : separate the image into RGB channels
% getfile : read file and save image pixel value into a matrix
% savefile : save matrix into a raw file
% rgb_disply : show the color image
% disply : show the image result
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Setup the pattern table
% S=1; K=2; ST=3; TK=4; STK=5;
% Set the conditional mark pattern
cpattern=zeros(11,11,2);
cpattern(1,:,1)=[32 128 4 1 0 0 0 0 0 0 0];   cpattern(1,:,2)=[1 1 1 1 0 0 0 0 0 0 0];
cpattern(2,:,1)=[8 64 16 2 0 0 0 0 0 0 0];    cpattern(2,:,2)=[1 1 1 1 0 0 0 0 0 0 0];
cpattern(3,:,1)=[40 96 192 144 20 6 3 9 0 0 0];   cpattern(3,:,2)=[1 1 1 1 1 1 1 1 0 0 0];
cpattern(4,:,1)=[72 80 18 10 41 224 148 7 0 0 0]; cpattern(4,:,2)=[4 4 4 4 5 5 5 5 0 0 0];
cpattern(5,:,1)=[200 73 112 42 104 208 22 11 0 0 0]; cpattern(5,:,2)=[3 3 3 3 3 3 3 3 0 0 0];
cpattern(6,:,1)=[201 116 232 105 240 212 150 23 15 43 0]; cpattern(6,:,2)=[3 3 5 5 5 5 5 5 5 5 0];
cpattern(7,:,1)=[233 244 151 47 0 0 0 0 0 0 0];   cpattern(7,:,2)=[5 5 5 5 0 0 0 0 0 0 0];
cpattern(8,:,1)=[107 248 214 31 0 0 0 0 0 0 0];   cpattern(8,:,2)=[5 5 5 5 0 0 0 0 0 0 0];
cpattern(9,:,1)=[235 111 252 249 246 215 159 63 0 0 0];   cpattern(9,:,2)=[5 5 5 5 5 5 5 5 0 0 0];
cpattern(10,:,1)=[239 253 247 191 0 0 0 0 0 0 0]; cpattern(10,:,2)=[5 5 5 5 0 0 0 0 0 0 0];
cpattern(11,:,1)=[251 254 223 127 0 0 0 0 0 0 0]; cpattern(11,:,2)=[2 2 2 2 0 0 0 0 0 0 0];

% Set the Shrink and Thin unconditional mark pattern (first part)
upattern_1=[32 128 2 8 40 96 192 144 20 6 3 9 112 200 73 42 0];
% Set the Shrink and Thin unconditional mark pattern (second part)
% (~temp(1,1)&&temp(1,3)&&~temp(2,1)&&temp(3,1)&&~temp(3,2)&&~temp(3,3)&&(temp(1,2)||temp(2,3))) || ...,
%     (temp(1,1)&&~temp(1,3)&&~temp(2,3)&&~temp(3,1)&&~temp(3,2)&&temp(3,3)&&(temp(1,2)||temp(2,1))) || ...,
%     (~temp(1,1)&&~temp(1,2)&&temp(1,3)&&~temp(2,3)&&temp(3,1)&&~temp(3,3)&&(temp(2,1)||temp(3,2))) || ...,
%     (temp(1,1)&&~temp(1,2)&&~temp(1,3)&&~temp(2,1)&&~temp(3,1)&&temp(3,3)&&(temp(2,3)||temp(3,2))) || ...,
%     (temp(1,1)&&temp(1,2)&&temp(2,1)) || ...,
%     (temp(1,2)&&~temp(1,3)&&temp(2,1)&&temp(2,3)&&~temp(3,2)&&~temp(3,3)) || ...,
%     (~temp(1,1)&&temp(1,2)&&temp(2,1)&&temp(2,3)&&~temp(3,1)&&~temp(3,2)) || ...,
%     (~temp(1,1)&&~temp(1,2)&&temp(2,1)&&temp(2,3)&&~temp(3,1)&&temp(3,2)) || ...,
%     (~temp(1,2)&&~temp(1,3)&&temp(2,1)&&temp(2,3)&&temp(3,2)&&~temp(3,3)) || ...,
%     (temp(1,2)&&temp(2,1)&&~temp(2,3)&&~temp(3,1)&&temp(3,2)&&~temp(3,3)) || ...,
%     (~temp(1,1)&&temp(1,2)&&~temp(1,3)&&temp(2,1)&&~temp(2,3)&&temp(3,2)) || ...,
%     (~temp(1,1)&&temp(1,2)&&~temp(1,3)&&~temp(2,1)&&temp(2,3)&&temp(3,2)) || ...,
%     (temp(1,2)&&~temp(2,1)&&temp(2,3)&&~temp(3,1)&&temp(3,2)&&~temp(3,3)) || ...,
%     (temp(1,1)&&temp(1,3)&&(temp(3,1)||temp(3,2)||temp(3,3))) || ...,
%     (temp(1,1)&&temp(3,1)&&(temp(1,3)||temp(2,3)||temp(3,3))) || ...,
%     (temp(3,1)&&temp(3,3)&&(temp(1,1)||temp(1,2)||temp(1,3))) || ...,
%     (temp(1,3)&&temp(3,3)&&(temp(1,1)||temp(2,1)||temp(3,1))) || ...,
%     (temp(1,2)&&~temp(1,3)&&~temp(2,1)&&temp(2,3)&&temp(3,1)&&~temp(3,2)) || ...,
%     (~temp(1,1)&&temp(1,2)&&temp(2,1)&&~temp(2,3)&&~temp(3,2)&&temp(3,3)) || ...,
%     (~temp(1,2)&&temp(1,3)&&temp(2,1)&&~temp(2,3)&&~temp(3,1)&&temp(3,2)) || ...,
%     (temp(1,1)&&~temp(1,2)&&~temp(2,1)&&temp(2,3)&&temp(3,2)&&~temp(3,3))

% Get the sample image in matrix
disp('Now getting image file information, please wait...');
sample=zeros(sampleylvl,samplexlvl,10);
for i=1:10
    [r_temp,g_temp,b_temp]=separate(samplexlvl,sampleylvl,getfile(samplexlvl*3,sampleylvl,samplepath{i}));
    temp=round(r_temp*0.21+g_temp*0.72+b_temp*0.07);
    for j=1:samplesize
        if temp(j)>127
            temp(j)=0;
        else temp(j)=1;
        end
    end
    sample(:,:,i)=temp;
    disply(samplexlvl,sampleylvl,temp*255,1);
end
% Get the testing image in matrix
testm=zeros(testylvl,testxlvl,10);
for i=1:10
    temp=(getfile(testxlvl*3,testylvl,testpath{i}));
    [r_temp,g_temp,b_temp]=separate(testxlvl,testylvl,temp);
    t=round(r_temp*0.21+g_temp*0.72+b_temp*0.07);
    for j=1:testsize
        if t(j)>127
            t(j)=0;
        else t(j)=1;
        end
    end
    testm(:,:,i)=t;
    disply(testxlvl,testylvl,t*255,1);
end
% Get the license image in matrix
lic=zeros(licylvl,licxlvl,7);
for i=1:7
    temp=(getfile(licxlvl*3,licylvl,licpath{i}));
    [r_temp,g_temp,b_temp]=separate(licxlvl,licylvl,temp);
    t=round(r_temp*0.21+g_temp*0.72+b_temp*0.07);
    for j=1:licsize
        if r_temp(j)==0 && g_temp(j)==0 && b_temp(j)==0
        elseif t(j)>90
            t(j)=0;
        else t(j)=1;
        end
    end
    lic(:,:,i)=t;
    disply(testxlvl,testylvl,t*255,i);
end
% Fill the holes
% for i=1:7
%     for m=1:licylvl-3
%         for n=1:licxlvl-3
%             temp=lic(m:m+3,n:n+3,i);
%             if (temp(1,2)&&temp(1,3)&&temp(2,1)&&temp(2,4)&&temp(3,1)&&temp(3,4)&&temp(4,2)&&temp(4,3)) && ...,
%                     (~temp(2,2)||~temp(2,3)||~temp(3,2)||~temp(3,3))
%                 lic(m:m+3,n:n+3,i)=ones(4);
%             elseif (~temp(1,2)&&~temp(1,3)&&~temp(2,1)&&~temp(2,4)&&~temp(3,1)&&~temp(3,4)&&~temp(4,2)&&~temp(4,3)) && ...,
%                     (temp(2,2)||temp(2,3)||temp(3,2)||temp(3,3))
%                 lic(m:m+3,n:n+3,i)=zeros(4);
%             end
%         end
%     end
%     disply(licxlvl,licylvl,lic(:,:,i)*255,1);
% end
disp('Finished!');

% Part A OCR Training------------------------------------------------------
sample_content={'k' '5' 'U' 'J' '8' '7' 'F' 'C' '9' '6'};
disp(sample_content)
% Thinning algorithm for training images
% for i=1:10
%     sample(:,:,i)=thinning(samplexlvl,sampleylvl,cpattern,upattern_1,sample(:,:,i));
% end
% symmetrical detection
[symInfo_s]=symmetry(samplexlvl,sampleylvl,sample,10);
% Calculate the bit quads for training images
disp('Now scan the training image for bit quads, please wait...');
[q_n]=bitquad(samplexlvl,sampleylvl,sample,10);
disp('Finished!');
q1_n=q_n{1};q2_n=q_n{2};q3_n=q_n{3};q4_n=q_n{4};q5_n=q_n{5};
a0=1/4*(q1_n+2*q2_n+3*q3_n+4*q4_n+2*q5_n);
p0=(q1_n+q2_n+q3_n+2*q5_n);
euler=1/4*(q1_n-q3_n-2*q5_n);
disp('Training images features calculation:');
% Get bonding box for training image
disp('Now calculating the bonding box information, please wait...');
sample_bb=bondingbox(samplexlvl,sampleylvl,sample,10);
disp('Finished!');
% Calculate the bondingbox information
bb_a0=zeros(1,10);bb_p0=zeros(1,10);
for i=1:10
    bb_a0(i)=sample_bb{i}(1)*sample_bb{i}(2);
    bb_p0(i)=(sample_bb{i}(1)+sample_bb{i}(2))*2;
end
an=a0./bb_a0;
pn=p0./bb_p0;
circ=4.*pi.*a0./(p0.^2);

% Part B OCR Testing: ideal case-------------------------------------------
% Right order
test_content={'K' '5' 'U' 'J' '8' '7' 'F' 'C' '9' '6'};
disp(test_content)
disp('Testing images features calculation:');
% Thinning algorithm for testing images
% for i=1:10
%     testm(:,:,i)=thinning(testxlvl,testylvl,cpattern,upattern_1,testm(:,:,i));
% end

% symmetrical detection
[symInfo_t]=symmetry(testxlvl,testylvl,testm,10);
% Calculate the bit quads for testing images
disp('Now scan the training image for bit quads, please wait...');
[q_n]=bitquad(testxlvl,testylvl,testm,10);
disp('Finished!');
q1_t_n=q_n{1};q2_t_n=q_n{2};q3_t_n=q_n{3};q4_t_n=q_n{4};q5_t_n=q_n{5};
a0_t=1/4*(q1_t_n+2*q2_t_n+3*q3_t_n+4*q4_t_n+2*q5_t_n);
p0_t=(q1_t_n+q2_t_n+q3_t_n+2*q5_t_n);
euler_t=1/4*(q1_t_n-q3_t_n-2*q5_t_n);
% Get bonding box for training image
disp('Now calculating the bonding box information, please wait...');
test_bb=bondingbox(testxlvl,testylvl,testm,10);
disp('Finished!');
% Calculate the bondingbox information
bb_a0_t=zeros(1,10);bb_p0_t=zeros(1,10);
for i=1:10
    bb_a0_t(i)=test_bb{i}(1)*test_bb{i}(2);
    bb_p0_t(i)=(test_bb{i}(1)+test_bb{i}(2))*2;
end
an_t=a0_t./bb_a0_t;
pn_t=p0_t./bb_p0_t;
circ_t=4.*pi.*a0_t./(p0_t.^2);
% Campare the testing images with training images
index=ones(1,10);

[m1,ind1]=min(circ_t);
[m2,ind2]=min(circ);

for i=1:10
    for j=1:10
        
    end
end
result=[test_content(ind1) '->' sample_content(ind2)];
disp(result)




% for i=1:10
%     diff=1;
%     for j=1:10
%         if euler_t(i)==euler(j)
%             temp=abs(an_t(i)-an(j))/an_t(i)*0.5+abs(circ_t(i)-circ(j))/circ_t(i)*0.3+abs(pn_t(i)-pn(j))/pn_t(i)*0.2;
%             if temp<diff
%                 diff=abs(an_t(i)-an(j))/an_t(i)*0.5+abs(circ_t(i)-circ(j))/circ_t(i)*0.3+abs(pn_t(i)-pn(j))/pn_t(i)*0.2;
%                 index(i)=j;
%             end
%         end
%     end
% end
% % Find the correction rate of this algorithm
% rate=0;content=cell(1,10);
% for i=1:10
%     temp=sample_content{index(i)};
%     content{i}=temp;
%     if temp==test_content{i};
%         rate=rate+1;
%     end
% end
% disp(['The order after OCR is: ' content{1} ',' content{2} ',' ...,
%     content{3} ',' content{4} ',' content{5} ',' content{6} ',' ...,
%     content{7} ',' content{8} ',' content{9} ',' content{10}]);
% disp(['The correction rate of this algorithm is: ' num2str(rate*10) '%']);

% % Part C OCR Application: license plate recognition------------------------
% % Right order
% lic_content={'5' 'M' 'N' 'S' '8' '9' '5'};    
% disp('License images features calculation:');   
% % Thinning algorithm for testing images
% for i=1:7
%     lic(:,:,i)=thinning(licxlvl,licylvl,cpattern,upattern_1,lic(:,:,i));
% end
% % Calculate the bit quads for testing images
% disp('Now scan the training image for bit quads, please wait...');
% [q_n]=bitquad(licxlvl,licylvl,lic,7);
% disp('Finished!');
% q1_l_n=q_n{1};q2_l_n=q_n{2};q3_l_n=q_n{3};q4_l_n=q_n{4};q5_l_n=q_n{5};
% a0_l=1/4*(q1_l_n+2*q2_l_n+3*q3_l_n+4*q4_l_n+2*q5_l_n);
% p0_l=(q1_l_n+q2_l_n+q3_l_n+2*q5_l_n);
% euler_l=1/4*(q1_l_n-q3_l_n-2*q5_l_n);   
% % Get bonding box for training image
% disp('Now calculating the bonding box information, please wait...');
% lic_bb=bondingbox(licxlvl,licylvl,lic,7);
% disp('Finished!');    
% % Calculate the bondingbox information
% bb_a0_l=zeros(1,7);bb_p0_l=zeros(1,7);
% for i=1:7
%     bb_a0_l(i)=lic_bb{i}(1)*lic_bb{i}(2);
%     bb_p0_l(i)=(lic_bb{i}(1)+lic_bb{i}(2))*2;
% end
% an_l=a0_l./bb_a0_l;
% pn_l=p0_l./bb_p0_l;
% circ_l=4.*pi.*a0_l./(p0_l.^2);
% % Campare the testing images with training images
% index=zeros(1,7);
% for i=1:7
%     diff=1;
%     for j=1:10
%         if euler_l(i)==euler(j)
%             temp=abs(an_l(i)-an(j))/an_l(i)*0.5+abs(circ_l(i)-circ(j))/circ_l(i)*0.3+abs(pn_l(i)-pn(j))/pn_l(i)*0.2;
%             if temp<diff
%                 diff=abs(an_l(i)-an(j))/an_l(i)*0.5+abs(circ_l(i)-circ(j))/circ_l(i)*0.3+abs(pn_l(i)-pn(j))/pn_l(i)*0.2;
%                 index(i)=j;
%             end
%         end
%     end
% end
% % Find the correction rate of this algorithm
% rate=0;content=cell(1,7);
% for i=1:7
%     temp=sample_content{index(i)};
%     content{i}=temp;
%     if temp==lic_content{i};
%         rate=rate+1;
%     end
% end
% disp(['The order after OCR is: ' content{1} ',' content{2} ',' ...,
%     content{3} ',' content{4} ',' content{5} ',' content{6} ',' ...,
%     content{7}]);
% disp(['The correction rate of this algorithm is: ' num2str(rate/7*100) '%']);
        
% % Part D OCR Application: real-life case----------------------------------- 
% % Get the plate image in matrix
% plate=zeros(500,1200);
% temp=getfile(platexlvl*3,plateylvl,platepath);
% [r_temp,g_temp,b_temp]=separate(platexlvl,plateylvl,temp);
% % [h,s,intensity]=rgbtohsi(platexlvl,plateylvl,platesize,r_temp,g_temp,b_temp);
% % h=round(h/2/pi*255);
% % h=he(platexlvl,plateylvl,platesize,h);
% % disply(platexlvl,plateylvl,h,1);
% plate=round(r_temp*0.229+g_temp*0.587+b_temp*0.114);
% % plate=he(platexlvl,plateylvl,platesize,plate);
% disply(platexlvl,plateylvl,plate,1);
% for i=1:platesize
%     if b_temp(i)-r_temp(i)<1 && b_temp(i)-g_temp(i)<1 && g_temp(i)-b_temp(i)<1
% %     if b_temp(i)>r_temp(i)+7 && b_temp(i)>g_temp(i)+7
% %             ((b_temp(i)>0.4*255 && b_temp(i)<0.7*255) || (b_temp(i)>0.2*255 && b_temp(i)<0.3*255))
%         plate(i)=0;
% %     else plate(i)=1;
%     end
% end
% disply(platexlvl,plateylvl,plate,2);        
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% All functions below
function [matrix]=he(xlvl,ylvl,allsize,pixel)
%--------------------------------------------------------------------------
% Input
% xlvl : the width of the image
% ylvl : the column of the image
% allsize : overall size of the image
% histo : the histogram distribution of the image
% pixel : the original matrix
% Output
% matrix : image after the histogram equalization
%--------------------------------------------------------------------------
% Get the histogram distribution of the image
histo=zeros(1,256);
for i=1:allsize;
    histo(round(pixel(i)+1))=histo(round(pixel(i)+1))+1;
end
matrix=zeros(ylvl,xlvl);
inhisto=zeros(1,256);
% Get the CDF of the histogram
inhisto(1)=histo(1);
for i=2:256
    inhisto(i)=inhisto(i-1)+histo(i);
end
% Generalize to a 256 gray level image
inhisto=inhisto/allsize*255;
% Equalize the image
for i=1:allsize
    matrix(i)=round(inhisto((pixel(i)+1)));
end
end
function [h,s,intensity]=rgbtohsi(church_xlvl,church_ylvl,churchsize,rpixel,gpixel,bpixel)
%--------------------------------------------------------------------------
% Input
% church_xlvl : the width of the image
% church_ylvl : the column of the image
% churchsize : overall size of the image
% rpixel : red channel matrix
% gpixel : green channel matrix
% bpixel : blue channel matrix
% Output
% h : hue channel of the image
% s : satuation channel of the image
% intensity : intensity channel of the image
%--------------------------------------------------------------------------
% Generalize the RGB information
r=zeros(church_ylvl,church_xlvl); g=zeros(church_ylvl,church_xlvl); b=zeros(church_ylvl,church_xlvl);
h=zeros(church_ylvl,church_xlvl);
s=zeros(church_ylvl,church_xlvl);
intensity=zeros(church_ylvl,church_xlvl);
temp=zeros(church_ylvl,church_xlvl);

% Convert the RGB into HSI format
% To avoid the NaN, add an eps to denominator
for i=1:churchsize
    r(i)=rpixel(i)/((rpixel(i)+gpixel(i)+bpixel(i))+eps);
    g(i)=gpixel(i)/((rpixel(i)+gpixel(i)+bpixel(i))+eps);
    b(i)=bpixel(i)/((rpixel(i)+gpixel(i)+bpixel(i))+eps);
end

for i=1:churchsize
    % Calculate H
    % To avoid the NaN, add an eps to denominator
    temp(i)=(0.5*(r(i)-g(i)+r(i)-b(i)))/(sqrt((r(i)-g(i))^2+(r(i)-b(i))*(g(i)-b(i)))+eps);
    if b(i)<=g(i)
        h(i)=acos(temp(i));
    else h(i)=2*pi-acos(temp(i));
    end
    % Calculate S
    if r(i)<=g(i) && r(i)<=b(i)
        s(i)=1-3*r(i);
    elseif g(i)<=r(i) && g(i)<=b(i)
        s(i)=1-3*g(i);
    else s(i)=1-3*b(i);
    end
    % Calculate I
    intensity(i)=(rpixel(i)+gpixel(i)+bpixel(i))/3/255;
end
end
function [size]=bondingbox(xlvl,ylvl,matrix,num)
%--------------------------------------------------------------------------
% Input
% xlvl : width of the image
% ylvl : column of the image
% matrix : the image to be scaned for bonding box information
% num : number of images in the cell
% Output
% size : the bonding box vertexed position information
%--------------------------------------------------------------------------
size=cell(1,num);
for i=1:num
    left=[1 xlvl];right=[1 1];
    top=[ylvl 1];bottom=[1 1];
    for m=1:ylvl
        for n=1:xlvl
            if matrix(m,n,i)==1
                if m<top(1)
                    top=[m n];
                end
                if m>bottom(1)
                    bottom=[m n];
                end
                if n<left(2)
                    left=[m n];
                end
                if n>right(2)
                    right=[m n];
                end
            end
        end
    end
    size{i}=[bottom(1)-top(1) right(2)-left(2)];
end
end


function [box]=bondingbox2(xlvl,ylvl,matrix)
%--------------------------------------------------------------------------
% Input
% xlvl : width of the image
% ylvl : column of the image
% matrix : the image to be scaned for bonding box information
% num : number of images in the cell
% Output
% size : the bonding box vertexed position information
%--------------------------------------------------------------------------
left=xlvl;right=1;
top=ylvl;bottom=1;
for m=1:ylvl
    for n=1:xlvl
        if matrix(m,n)==1
            if m<top
                top=m;
            end
            if m>bottom
                bottom=m;
            end
            if n<left
                left=n;
            end
            if n>right
                right=n;
            end
        end
    end
end
box=[top bottom left right];

end


function [t_pixel]=thinning(xlvl,ylvl,cpattern,upattern_1,tpixel)
%--------------------------------------------------------------------------
% Input
% xlvl : width of the image
% ylvl : column of the image
% cpattern : the conditional pattern
% upattern_1 : the unconditional pattern for thinning and shrinking
% tpixel : the original image for thinning
% Output
% t_pixel : the output after thinning
%--------------------------------------------------------------------------
% Setup a new matrix same as the original one for processing
t_pixel=tpixel;
% Scan the image with 3*3 window
count=1;loops=0;
while count~=0
    loops=loops+1;
    % Initialize the markmatrix
    markmatrix=zeros(ylvl,xlvl);
    tm=t_pixel;
    % Extent the matrix with one line around for 3*3 window scan
    tpixel_ex=zeros(ylvl+2,xlvl+2);
    tpixel_ex(1:ylvl+2,1)=zeros(ylvl+2,1);
    tpixel_ex(1:ylvl+2,xlvl+2)=zeros(ylvl+2,1);
    tpixel_ex(1,2:xlvl+1)=zeros(1,xlvl);
    tpixel_ex(ylvl,2:xlvl+1)=zeros(1,xlvl);
    tpixel_ex(2:ylvl+1,2:xlvl+1)=t_pixel(1:ylvl,1:xlvl);
    % Flag for whether end of thinning or not
    count=0;
    % Conditional processing
    for m=1:ylvl
        for n=1:xlvl
            if t_pixel(m,n)==1
                temp=tpixel_ex(m:m+2,n:n+2);
                % Find the bond of this window
                [bond]=countbond(temp);
                % Calculate the digit of this window
                [num]=countdigit(temp);
                if bond<12 && bond>0
                    % Match the window with the pattern table of the same bond
                    for i=1:11
                        if ((cpattern(bond,i,2)==3 || cpattern(bond,i,2)==4 || cpattern(bond,i,2)==5) && cpattern(bond,i,1)==num)
                            break
                        end
                    end
                    if i==11
                        markmatrix(m,n)=0;
                    else
                        count=count+1;
                        markmatrix(m,n)=1;
                        tm(m,n)=0;
                    end
                else
                    markmatrix(m,n)=0;
                end
            else
                markmatrix(m,n)=0;
            end
        end
    end
    % Unconditional processing
    % Initialize the output matrix
    t_pixel=zeros(ylvl,xlvl);
    % Extent the matrix with one line around for 3*3 window scan
    markmatrix_ex=zeros(ylvl+2,xlvl+2);
    markmatrix_ex(1:ylvl+2,1)=zeros(ylvl+2,1);
    markmatrix_ex(1:ylvl+2,xlvl+2)=zeros(ylvl+2,1);
    markmatrix_ex(1,2:xlvl+1)=zeros(1,xlvl);
    markmatrix_ex(ylvl,2:xlvl+1)=zeros(1,xlvl);
    markmatrix_ex(2:ylvl+1,2:xlvl+1)=markmatrix(1:ylvl,1:xlvl);
    % Scan the image
    for m=1:ylvl
        for n=1:xlvl
            if markmatrix(m,n)==1
                temp=markmatrix_ex(m:m+2,n:n+2);
                % Calculate the digit of this window
                [num]=countdigit(temp);
                flag=0;
                if  (~temp(1,1)&&temp(1,3)&&~temp(2,1)&&temp(3,1)&&~temp(3,2)&&~temp(3,3)&&(temp(1,2)||temp(2,3))) || ...,
                        (temp(1,1)&&~temp(1,3)&&~temp(2,3)&&~temp(3,1)&&~temp(3,2)&&temp(3,3)&&(temp(1,2)||temp(2,1))) || ...,
                        (~temp(1,1)&&~temp(1,2)&&temp(1,3)&&~temp(2,3)&&temp(3,1)&&~temp(3,3)&&(temp(2,1)||temp(3,2))) || ...,
                        (temp(1,1)&&~temp(1,2)&&~temp(1,3)&&~temp(2,1)&&~temp(3,1)&&temp(3,3)&&(temp(2,3)||temp(3,2))) || ...,
                        (temp(1,1)&&temp(1,2)&&temp(2,1)) || ...,
                        (temp(1,2)&&temp(1,3)&&temp(2,3)&&~temp(1,1)&&~temp(2,1)&&~temp(3,1)&&~temp(3,2)&&~temp(3,3)) || ...,
                        (temp(1,2)&&~temp(1,3)&&temp(2,1)&&temp(2,3)&&~temp(3,2)&&~temp(3,3)) || ...,
                        (~temp(1,1)&&temp(1,2)&&temp(2,1)&&temp(2,3)&&~temp(3,1)&&~temp(3,2)) || ...,
                        (~temp(1,1)&&~temp(1,2)&&temp(2,1)&&temp(2,3)&&~temp(3,1)&&temp(3,2)) || ...,
                        (~temp(1,2)&&~temp(1,3)&&temp(2,1)&&temp(2,3)&&temp(3,2)&&~temp(3,3)) || ...,
                        (temp(1,2)&&temp(2,1)&&~temp(2,3)&&~temp(3,1)&&temp(3,2)&&~temp(3,3)) || ...,
                        (~temp(1,1)&&temp(1,2)&&~temp(1,3)&&temp(2,1)&&~temp(2,3)&&temp(3,2)) || ...,
                        (~temp(1,1)&&temp(1,2)&&~temp(1,3)&&~temp(2,1)&&temp(2,3)&&temp(3,2)) || ...,
                        (temp(1,2)&&~temp(2,1)&&temp(2,3)&&~temp(3,1)&&temp(3,2)&&~temp(3,3)) || ...,
                        (temp(1,1)&&temp(1,3)&&(temp(3,1)||temp(3,2)||temp(3,3))) || ...,
                        (temp(1,1)&&temp(3,1)&&(temp(1,3)||temp(2,3)||temp(3,3))) || ...,
                        (temp(3,1)&&temp(3,3)&&(temp(1,1)||temp(1,2)||temp(1,3))) || ...,
                        (temp(1,3)&&temp(3,3)&&(temp(1,1)||temp(2,1)||temp(3,1))) || ...,
                        (temp(1,2)&&~temp(1,3)&&~temp(2,1)&&temp(2,3)&&temp(3,1)&&~temp(3,2)) || ...,
                        (~temp(1,1)&&temp(1,2)&&temp(2,1)&&~temp(2,3)&&~temp(3,2)&&temp(3,3)) || ...,
                        (~temp(1,2)&&temp(1,3)&&temp(2,1)&&~temp(2,3)&&~temp(3,1)&&temp(3,2)) || ...,
                        (temp(1,1)&&~temp(1,2)&&~temp(2,1)&&temp(2,3)&&temp(3,2)&&~temp(3,3))
                    flag=1;
                end
                % Match the window with the pattern table of the same bond
                for i=1:length(upattern_1)
                    if upattern_1(i)==num
                        break
                    end
                end
                if i~=length(upattern_1) || flag
                    t_pixel(m,n)=1;
                else
                    t_pixel(m,n)=0;
                end
            else
                t_pixel(m,n)=0;
            end
        end
    end
    t_pixel=t_pixel+tm;
    disply(xlvl,ylvl,t_pixel*255,5)
end
end
function [num]=countdigit(matrix)
%--------------------------------------------------------------------------
% Input
% matrix : the 3*3 window to calculate the digit
% Output
% bond : the digit of the window
%--------------------------------------------------------------------------
num=0;
for m=1:3
    for n=1:3
        if m==2 && n==2
        else
            num=num*2+matrix(m,n);
        end
    end
end
end
function [bond]=countbond(matrix)
%--------------------------------------------------------------------------
% Input
% matrix : the 3*3 window to calculate the bond
% Output
% bond : the bond of the window
%--------------------------------------------------------------------------
bond=0;
for m=1:3
    for n=1:3
        if matrix(m,n)==1
            if m+n==3 || m+n==5
                bond=bond+2;
            elseif m+n==2 || m+n==6 || (m+n==4 && m~=2 && n~=2)
                bond=bond+1;
            end
        end
    end
end
end
function [q_n]=bitquad(xlvl,ylvl,matrix,num)

q1={[1 0;0 0] [0 1;0 0] [0 0;0 1] [0 0;1 0]};
q2={[1 1;0 0] [0 1;0 1] [0 0;1 1] [1 0;1 0]};
q3={[1 1;0 1] [0 1;1 1] [1 0;1 1] [1 1;1 0]};
q4={[1 1;1 1] [1 1;1 1] [1 1;1 1] [1 1;1 1]};
qd={[1 0;0 1] [0 1;1 0] [1 0;0 1] [0 1;1 0]};
%--------------------------------------------------------------------------
% Input
% xlvl : width of the image
% ylvl : column of the image
% m : images to be scaned
% num : the number of images in the cell
% qn : the bit quad patterns
% Output
% q_n : the bit quad number
%--------------------------------------------------------------------------

q1_n=zeros(1,num);q2_n=zeros(1,num);q3_n=zeros(1,num);q4_n=zeros(1,num);q5_n=zeros(1,num);
% Scan the image for bit quads calculation
for i=1:num
    for m=1:ylvl-1
        for n=1:xlvl-1
            temp=matrix(m:m+1,n:n+1,i);
            for j=1:4
                if temp==q1{j}
                    q1_n(i)=q1_n(i)+1;
                    break
                elseif temp==q2{j}
                    q2_n(i)=q2_n(i)+1;
                    break
                elseif temp==q3{j}
                    q3_n(i)=q3_n(i)+1;
                    break
                elseif temp==q4{j}
                    q4_n(i)=q4_n(i)+1;
                    break
                elseif temp==qd{j}
                    q5_n(i)=q5_n(i)+1;
                    break
                end
            end
        end
    end
end
q_n={q1_n q2_n q3_n q4_n q5_n};
end
function [rpixel,gpixel,bpixel]=separate(xlvl,ylvl,opixel)
%--------------------------------------------------------------------------
% Input
% xlvl : width of the image
% ylvl : column of the image
% opixel :  the matrix storing the RGB image pixel information
% Output
% rpixel : the red channel of the image
% gpixel : the green channel of the image
% bpixel : the blue channel fo the image
%--------------------------------------------------------------------------
rpixel=zeros(ylvl,xlvl);
gpixel=zeros(ylvl,xlvl);
bpixel=zeros(ylvl,xlvl);

for m=1:ylvl
    for n=1:xlvl
        rpixel(m,n)=opixel(((m-1)*xlvl+n)*3-2);
        gpixel(m,n)=opixel(((m-1)*xlvl+n)*3-1);
        bpixel(m,n)=opixel(((m-1)*xlvl+n)*3);
    end
end
end
function [pixel]=getfile(xlvl,ylvl,filename)
%--------------------------------------------------------------------------
% Input
% xlvl : width of the image
% ylvl : column of the image
% filename : the filepath of the image to be used below
% Output
% pixel : the matrix storing the pixel information of the image
%--------------------------------------------------------------------------
fid=fopen(filename,'rb');

% Get all the pixels from the image
pixel = fread(fid,[ylvl,xlvl], 'uint8');

% Close file
fclose(fid);
end
function savefile(pixel,filename)
%--------------------------------------------------------------------------
% Input
% pixel : the gray level matrix for storage
% filename : the path for the image to save
%--------------------------------------------------------------------------
% Save gray level image
pixel=pixel';
fid=fopen(filename,'wb');

fwrite(fid,pixel, 'uint8');

fclose(fid);
end
function rgb_disply(xlvl,ylvl,rpixel,gpixel,bpixel,num)
%--------------------------------------------------------------------------
% Input
% xlvl : the width of the image
% ylvl :  the column of the image
% rpixel : red channel of the image
% gpixel : green channel of the image
% bpixel : blue channel of the image
% num : the current figure number
%--------------------------------------------------------------------------
% Display the color image
dis=zeros(ylvl,xlvl,3);
dis(:,:,1)=rpixel/255;
dis(:,:,2)=gpixel/255;
dis(:,:,3)=bpixel/255;
figure(num);
image(dis);
end


function [symInfo]=symmetry(xlvl,ylvl,image,num)
symInfo=zeros(num,2);
%orientation:
%   0=N/A
%   1=vertical symmetry
%   2=horizontal symmetry

for count=1:num
    tempimg=image(:,:,count);
    box=bondingbox2(xlvl,ylvl,tempimg);
    top=box(1);
    bottom=box(2);
    left=box(3);
    right=box(4);
    v_mid=floor((bottom-top)/2)+top;
    h_mid=floor((right-left)/2)+left;
    total=sum(sum(tempimg(top:bottom,left:right)));%total 1's
    % test vertical
    temp=zeros(v_mid-1,xlvl);
    for i=top:v_mid-1
        for j=left:right
            if tempimg(i,j)~=tempimg(v_mid-i+v_mid,j)
                temp(i,j)=1;
            end
        end
    end
    total_error=sum(sum(temp));
    
    if total_error/total<=0.25 % 25percent tollerance
        sym=true;
        orientation=1;
        symInfo(count,1)=sym;
        symInfo(count,2)=orientation;
        
    else
        %test horizonal
        temp=zeros(ylvl,h_mid-1);
        for i=top:bottom
            for j=left:h_mid-1
                if tempimg(i,j)~=tempimg(i,h_mid-j+h_mid)
                    temp(i,j)=1;
                end
            end
        end
        total_error=sum(sum(temp));
        
        if total_error/total<=0.25% 25percent tollerance
            sym=true;
            orientation=2;
            symInfo(count,1)=sym;
            symInfo(count,2)=orientation;
            
        else %else return [false,0]
            symInfo(count,1)=0;
            symInfo(count,2)=0;
        end
 
    end

end

end


function disply(xlvl,ylvl,m,num)
%--------------------------------------------------------------------------
% Input
% xlvl : the width of the image
% ylvl : the column of the image
% m : the pixel value matrix
% num : current figure number
%--------------------------------------------------------------------------
% Display the gray level image
dis=zeros(ylvl,xlvl);
dis(:,:)=m/255;
figure(num)
imshow(dis);
end