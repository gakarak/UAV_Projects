close all;
clear all;

fcsv='data_r109_0/video.drone_115.3.15.11.9.0.avi.csv';
% % fcsv='data_r109_1/video.drone_115.3.15.13.1.24.avi.csv';

dataa=importdata(fcsv);
data=dataa.data;


pTime=data(:,2);
pBatt=data(:,4);
pAngRPY=data(:,5:7);
pVelXYZ=data(:,8:10);
pVelXYZC=sqrt(pVelXYZ(:,1).^2+pVelXYZ(:,2).^2+pVelXYZ(:,3).^2);
pVelM=data(:,11);
pZ=data(:,12);
pAXYZ=data(:,13:15);

figure,
subplot(2,2,1), plot(pTime, pBatt), title('Battery(t)')
subplot(2,2,2), plot(pTime, pVelM),title('VelMag(t)')
subplot(2,2,3), plot(pTime, pZ),    title('Z(t)')
subplot(2,2,4), plot(pTime, pAngRPY(:,3)),    title('Yaw(t)')

%%
figure,
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
    plot(pTime, cos(pAngRPY(:,3))), legend({'Roll', 'Pitch', 'Yaw'})
    hold off
subplot(1,3,3),
    hold all,
    plot(pTime, pAXYZ(:,1)),
    plot(pTime, pAXYZ(:,2)),
    plot(pTime, pAXYZ(:,3)), legend({'ax', 'ay', 'az'})
    hold off

%%
dT=(pTime(2:end)-pTime(1:end-1))/1000;
dVXYZ=pVelXYZ(1:end-1,:);
dYaw=pAngRPY(1:end-1,3)-pAngRPY(1,3);
dXYZ=cumsum(dVXYZ.*repmat(dT,1,3));
dXX=cumsum( dVXYZ(:,1).*dT.*cos(dYaw) - dVXYZ(:,2).*dT.*sin(dYaw) );
dYY=cumsum( dVXYZ(:,1).*dT.*sin(dYaw) + dVXYZ(:,2).*dT.*cos(dYaw) );

figure,
subplot(2,2,1), plot(dXYZ), legend('X','Y','Z')
subplot(2,2,2),
    hold all
    plot(pTime(1:end-1), dXYZ(:,3)),
    plot(pTime, pZ-pZ(1))
    legend('Z-Estimated','Z-Drone')
subplot(2,2,3),
    plot(dXX,dYY), title('XY-Estimated')
