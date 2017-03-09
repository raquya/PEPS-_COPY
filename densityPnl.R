data=scan("./MementisLibrary/build/resultHedging.txt")
plot(density(data,na.rm = TRUE), col="red")
str=sprintf("Espérance %f\n Variance %f",mean(data),var(data))
names<-c(str)
legend("topleft", names, cex=0.8, col="red", 
       lty=1:3, lwd=2, bty="n")