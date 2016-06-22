% first step - rescale image and count Mixture of Gaussians for the next step - cranium removal

clear all
close all
NoOfGaussians = 3;
Percentil = 99.9;

% read files from
Ordner = 'C:\Users\Zuzana\Documents\MATLAB\krok1BrainStatis\data\';
OrdnerSearch = sprintf('%s*.png',Ordner);
Files = dir(OrdnerSearch);
size(Files)

%time measurements
timeCEsum = 0;
timeMOGsum = 0;
count = 0;

 for  CountFiles =1:size(Files)
 Files(CountFiles).name
 FileNameImage = sprintf('%s%s',Ordner, Files(CountFiles).name)
 
 %open the file
 B= imread( FileNameImage);
 %imshow(B);
 %figure;
 count = count + 1;
 tic;
	%rescaling
     imgMin = prctile(B(:),35); % skusit zmenit na 20-25
     imgMax = prctile(B(:),99.9);
     Bmin = B-imgMin;
     Bmax = double(Bmin)/(double(imgMax)-double(imgMin))*255;
     Bscaled = uint8(Bmax); %rescaled image
 timeCE = toc;
 timeCEsum = timeCEsum + timeCE;
 
 FileNameImageRescaled = sprintf('%s_rescaled.png',FileNameImage(1:length(FileNameImage)-4));
 imwrite(Bscaled, FileNameImageRescaled);
 %imshow(Bscaled);
 B = Bscaled;
 %pause;
 
 tic;
 
% Mixture of Gaussians
N= rand(length(B(:)),1);
BN= double(B(:))+N;
gm = gmdistribution.fit(BN,NoOfGaussians,'Replicates',3);
gm.Sigma
gm.mu
[maxV, maxInd]= max(gm.Sigma)
maxMu = floor(gm.mu(maxInd))

BinMax = uint8( B < maxMu );
[SortSigma,SortSigmaIdx] = sort(gm.Sigma,'descend')
[SortMu,SortMuIdx] = sort(gm.mu,'descend')
% imshow(BinMax*256);
P99 = prctile(B(:),Percentil)
% figure
% hist(double(B(:)),256)
% pause
timeMOG = toc;
timeMOGsum = timeMOGsum + timeMOG;

FileNameImageMax = sprintf('%s_rescaled_statis.txt',FileNameImage(1:length(FileNameImage)-4));
fid = fopen(FileNameImageMax, 'w')
fprintf(fid,'%d\n', NoOfGaussians);

%save results of Mixture of Gaussians
for ind = 1:NoOfGaussians
    fprintf(fid,'%4.1f\t', (floor(SortMu(ind))));
end
fprintf(fid,'\n');
for ind = 1:NoOfGaussians
    fprintf(fid,'%d\t', (SortMuIdx(ind)));
end
fprintf(fid,'\n');
for ind = 1:NoOfGaussians
    fprintf(fid,'%4.1f\t', (floor(SortSigma(ind))));
end
fprintf(fid,'\n');
for ind = 1:NoOfGaussians
    fprintf(fid,'%d\t', (SortSigmaIdx(ind)));
end
fprintf(fid,'\n');
fprintf(fid,'%3.1f\t%5.1f', P99,Percentil );
fprintf(fid,'\n');
fprintf(fid,'%3.8f', timeCE);
fprintf(fid,'\n');
fprintf(fid,'%3.8f', timeMOG);
fclose(fid);
 end
 
 file = fopen('satis.txt', 'w');
 
 fprintf(fid,'Average contrast enhancement time: %3.4f', timeCEsum/count );
 fprintf(fid,'\n');
 fprintf(fid,'Average mixture of gaussians time: %3.4f', timeMOGsum/count );
 
 fclose(file);
 