prix<-scan("./MementisLibrary/dataPortefeuilleCouverture.txt")
produit<-scan("./MementisLibrary/dataProduit.txt")
date<-scan("./MementisLibrary/dataDate.txt")
dateRebalancement<- seq(12,144,12)
pnl<-scan("./MementisLibrary/entrt.txt")
#tracer du graphe des courbes du portefeuille de couverture et du prix du produit
plot(dateRebalancement,pnl, col="blue", type="l")
names<-c("Pnl")
plot_colors <- c("blue")
legend("topleft", names, cex=0.8, col=plot_colors, 
       lty=1:3, lwd=2, bty="n")

