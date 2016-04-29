close all;
clear all;


% % fcsv='../data/data_r109_0/video.drone_115.3.15.11.10.19.avi.csv';
% % fcsv='../data/data_r109_0/video.drone_115.3.15.11.4.23.avi.csv';
% % fcsv='../data/data_r109_0/video.drone_115.3.15.11.9.0.avi.csv';
% % fcsv='../data/data_r109_1/video.drone_115.3.15.13.1.24.avi.csv';
% % fcsv='../data/data_r109_2/video.drone_115.3.15.16.39.19.avi.csv';
fcsv='../data/data_r109_2/video.drone_115.3.15.16.35.30.avi.csv';


dataa=importdata(fcsv);
data=dataa.data;


pTime=data(:,2);
try
    pTime2=data(:,16)/1000;
catch
    pTime2=data(:,2);
end
pBatt=data(:,4);
pAngRPY=data(:,5:7);
pVelXYZ=data(:,8:10);
pVelXYZC=sqrt(pVelXYZ(:,1).^2+pVelXYZ(:,2).^2+pVelXYZ(:,3).^2);
pVelM=data(:,11);
pZ=data(:,12);
pAXYZ=data(:,13:15);

hf1=figure;
subplot(2,2,1), plot(pTime, pBatt), title('Battery(t)')
subplot(2,2,2), plot(pTime, pVelM),title('VelMag(t)')
subplot(2,2,3), plot(pTime, pZ),    title('Z(t)')
subplot(2,2,4), plot(pTime, pAngRPY(:,3)),    title('Yaw(t)')

%%
hf2=figure;
subplot(1,3,1),
    hold all,
    plot(pTime, pVelXYZ(:,1)),
    plot(pTime, pVelXYZ(:,2)),
    plot(pTime, pVelXYZ(:,3)), legend({'Vx', 'Vy', 'Vz'})
    hold off
subplot(1,3,2),
    hold all,
    plot(pTime, pAngRPY(:,1)),
    plot(pTime, pAngRPY(:,2)),
    plot(pTime, pAngRPY(:,3)), legend({'Roll', 'Pitch', 'Yaw'})
    hold off
subplot(1,3,3),
    hold all,
    plot(pTime, pAXYZ(:,1)),
    plot(pTime, pAXYZ(:,2)),
    plot(pTime, pAXYZ(:,3)), legend({'ax', 'ay', 'az'})
    hold off

%%
% % dT=(pTime(2:end)-pTime(1:end-1))/1000;
dT=(pTime2(2:end)-pTime2(1:end-1))/1000;
dVXYZ=pVelXYZ(1:end-1,:);
dYaw=pAngRPY(1:end-1,3)-pAngRPY(1,3);
dPitch=pAngRPY(1:end-1,2);
dXYZ=cumsum(dVXYZ.*repmat(dT,1,3));
dXX=cumsum( dVXYZ(:,1).*dT.*cos(dYaw) - dVXYZ(:,2).*dT.*sin(dYaw) );
dYY=cumsum( dVXYZ(:,1).*dT.*sin(dYaw) + dVXYZ(:,2).*dT.*cos(dYaw) );
dZZ=cumsum( dVXYZ(:,3).*dT.*cos(dPitch) - dVXYZ(:,1).*dT.*sin(dPitch) );

hf3=figure;
subplot(2,2,1),
    plot(dXYZ), legend('X','Y','Z')
subplot(2,2,2),
    hold all
    plot(pTime(1:end-1), dXYZ(:,3)),
    plot(pTime, pZ-pZ(1))
    plot(pTime(1:end-1), dZZ)
    legend('Z-Estimated','Z-Drone', 'Z-Estimated (Pitch)')
    hold off
subplot(2,2,3),
    hold all
    plot(dXX,dYY), title('XY-Estimated');
% %     plot(dXYZ(:,1),dXYZ(:,2))
% %     legend('XY-Estimated', 'XY-Estimated without Yaw')
    hold off
subplot(2,2,4),
    grid on;
    plot3(dXX,dYY,dXYZ(:,3)), title('XY-Estimated');
    xlabel('X'), ylabel('Y'), zlabel('Z');
    grid off;

%%
saveas(hf1, [fcsv, '_fig1.png'], 'png');
saveas(hf2, [fcsv, '_fig2.png'], 'png');
saveas(hf3, [fcsv, '_fig3.png'], 'png');


