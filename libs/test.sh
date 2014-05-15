stringZ=$(date -u +"%H%M%S.h264")
raspivid -w 640 -h 480 -t 5000 -n -o ${stringZ}
#raspivid -t 10000 -n -o video.h264
#raspivid -w 640 -h 480 -t 1800000 -fps 2 -n -o video.h264

#connect via scp
scp -i ~/.ssh/key ${stringZ} jonas@10.200.18.52:/home/jonas/
rm ${stringZ}
