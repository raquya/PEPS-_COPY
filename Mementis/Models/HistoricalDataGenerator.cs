using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text;
using System.Threading.Tasks;
using Excel = Microsoft.Office.Interop.Excel;
using System.Runtime.InteropServices;
using System.IO;

namespace Mementis.Models
{
    public class HistoricalDataGenerator
    {
        /* Seulement date entière !!! */
        public double[] GetData(double currentDate)
        {
            //Pour que cette partie marche vous devez ajouter une reférence dans le projet: Aller dans l'onglet "Projet" puis "Ajouter une référence" puis dans l'onglet "COM"
            // puis vous chercher Excel dans la barre de recherche a droite puis vous devez cocher "Microsoft Excel 14.0 Object Library" et cliquer sur ok
            //Les cours de nos actions sont dans le fichier CoursAction j'ai mis le chemin en relatif donc vous avez rien a changer

            var wanted_path = Path.GetDirectoryName(Path.GetDirectoryName(
                System.IO.Path.GetDirectoryName(
                    System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase)));

            int ligneOffSet = 3;
            int colonneOffSet = 2;
            int nbActions = 25;
            int nbPrixMax = 9; // Pour l'instant
            int nbPrix = (int) Math.Ceiling(currentDate) + 1;

            int nbData = nbPrix * nbActions;
            double[] prixData = new double[nbData];
           
            Excel.Application xlApp = new Excel.Application();
            Excel.Workbook xlWorkbook = xlApp.Workbooks.Open(@wanted_path+"\\CoursActions.xlsx");
            Excel._Worksheet xlWorksheet = xlWorkbook.Sheets[1];
            Excel.Range xlRange = xlWorksheet.UsedRange;
          
            for (int i = 0; i < nbPrix; i++)
            {
                for (int j = 0; j < nbActions; j++)
                {
                    prixData[i * 25 + j] = xlRange.Cells[i + ligneOffSet, j + colonneOffSet].Value2;
                    //System.Diagnostics.Debug.WriteLine("Action ["+j+"] en date " + i + " : " + prixData[i * 25 + j]);
                }
            }

            GC.Collect();
            GC.WaitForPendingFinalizers();

            Marshal.ReleaseComObject(xlRange);
            Marshal.ReleaseComObject(xlWorksheet);

            //ferme Excel
            xlWorkbook.Close();
            Marshal.ReleaseComObject(xlWorkbook);

            //Quitte apli
            xlApp.Quit();
            Marshal.ReleaseComObject(xlApp);

            return prixData;
        }
    }
}