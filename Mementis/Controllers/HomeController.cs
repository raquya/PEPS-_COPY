using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using Mementis.Models;
using MementisWrapper;

namespace Mementis.Controllers
{
    public class HomeController : Controller
    {
        /* Attention aux param global !!!
         * (Ils sont communs à tous les clients Web) */
        private int nbSamples = 1000; // Nombre de ré-évaluation des prix des sous-jacents
        string format = "MM/dd/yyyy";
        string[] nomActions = new string[25] { "1", "2", "3" , "4", "5", "6" , "7", "8", "9" , "10", "11", "12" , "13", "14", "15" ,"16", "17", "18" , "19", "20", "21", "22", "23", "24", "25" };

        private void lib()
        {
            /* Necessaire pour l'accès aux librairies !!! */
            String _path = String.Concat(System.Environment.GetEnvironmentVariable("PATH"), ";", System.AppDomain.CurrentDomain.RelativeSearchPath);
            System.Environment.SetEnvironmentVariable("PATH", _path, EnvironmentVariableTarget.Process);
        }

        // GET: Home
        public ActionResult Main()
        {
            System.Diagnostics.Debug.WriteLine("********************************************************");
            System.Diagnostics.Debug.WriteLine("                          MAIN                          ");
            System.Diagnostics.Debug.WriteLine("");

            lib();

            return View("Home");
        }

        // currentDate doit être entre 0 et 12
        [HttpPost]
        public ActionResult Simulation(Double currentDate, Boolean isHistorical, Boolean nbHedgingDate, String command)
        {
            if (command.Equals("Mini-Test-Web"))
            {
                return Test();
            }
            else
            {
                DateTime startDate = new DateTime(2007, 4, 12);
                ViewData["startDate"] = startDate.ToString(format);
                ViewData["endDate"] = startDate.AddYears(12).ToString(format);

                // Ex : currentDate = 7.82
                int year = (int)Math.Floor(currentDate); // 7 years
                int month = (int)Math.Floor((currentDate - year) * 12); // 9 months
                int day = (int)Math.Floor((currentDate - month) * 30); // 25 days

                ViewData["currentDate"] = startDate.AddYears(year).AddMonths(month).AddDays(day).ToString(format);
                ViewData["dataPastType"] = (isHistorical)?"Historique":"Générées";

                // Génération du past
                double[] past = null;
                if (isHistorical)
                {
                    HistoricalDataGenerator generator = new HistoricalDataGenerator();
                    past = generator.GetData(currentDate);
                }


                if (command.Equals("PriceAndHedge"))
                {
                    int nbPdtCouv = 0; // Nombre de sous-jacent utilisés pour la couverture, Exemple 27 : 25 actions + 2 forwards
                    double[] delta;
                    double[] icDelta;
                    double prix = 0.0;
                    double icPrix = 0.0;

                    // Paramètres IN : past
                    // Paramètres OUT : nbPdtCouv, delta, icDelta, prix, icPrix
                    WrapperPriceAndHedge w = new WrapperPriceAndHedge();
                    try
                    {
                        w.priceAndHedgeAtCurrentDate(currentDate, past, nbSamples, isHistorical);
                        nbPdtCouv = w.getNbPdtCouv();
                        delta = w.getDelta();
                        icDelta = w.getIcDelta();
                        prix = w.getPrix();
                        icPrix = w.getIcPrix();
                    }
                    catch (Exception ex)
                    {
                        System.Diagnostics.Debug.WriteLine(ex);
                        ViewData["Exception"] = ex;
                        return View("Error");
                    }

                    ViewData["nbPdtCouv"] = nbPdtCouv;
                    ViewData["delta"] = delta;
                    ViewData["icDelta"] = icDelta;
                    ViewData["prix"] = prix;
                    ViewData["icPrix"] = icPrix;
                    ViewData["nomActions"] = nomActions;

                    return View("PriceAndHedge");

                }
                else if (command.Equals("PnL"))
                {
                    int nbHedgingDateTmp = (nbHedgingDate)?12:24;
                    double[] valPtfCouv;
                    double[] prixPdt;
                    double pnl = 0.0;

                    // Paramètres IN : past
                    // Paramètres OUT : valPtfCouv, prixPdt, pnl
                    WrapperPnL w = new WrapperPnL();
                    try
                    {
                        w.PnL(nbHedgingDateTmp, currentDate, 0, past, nbSamples, isHistorical);
                        nbHedgingDateTmp = w.getNbHedgingDate();
                        valPtfCouv = w.getValPtfCouv();
                        prixPdt = w.getPrixPdt();
                        pnl = w.getPnL();
                    }
                    catch (Exception ex)
                    {
                        System.Diagnostics.Debug.WriteLine(ex);
                        ViewData["Exception"] = ex;
                        return View("Error");
                    }

                    ViewData["nbHedgingDate"] = nbHedgingDateTmp;
                    ViewData["valPtfCouv"] = valPtfCouv;
                    ViewData["prixPdt"] = prixPdt;
                    ViewData["pnl"] = pnl;
                 
                    double cptDate = 31536000000 * (30 + currentDate);
                    String dataChart1 = "[";
                    String dataChart2 = "[";
                    for (int i = 0; i < nbHedgingDateTmp-1; i++)
                    {
                        dataChart1 += "[" + cptDate + "," + prixPdt[i] + "],";
                        dataChart2 += "[" + cptDate + "," + valPtfCouv[i] + "],";
                        cptDate += (nbHedgingDate)?31536000000:15768000000;
                    }
                    dataChart1 += "[" + cptDate + "," + prixPdt[nbHedgingDateTmp - 1] + "]]";
                    dataChart2 += "[" + cptDate + "," + valPtfCouv[nbHedgingDateTmp - 1] + "]]";
                    ViewData["dataChart1"] = dataChart1;
                    ViewData["dataChart2"] = dataChart2;
                    return View("PnL");

                }
                else
                {
                    return View("Error");
                }
            }
        }


        public ActionResult Test()
        {
            double random = -100000.0;
            double m = -999.0;
            lib();
            WrapperTest w = new WrapperTest();

            try
            {
                // La fonction appelée en C++ Natif, via la librairie PNL,
                // tire un nombre tiré aléatoirement selon une loi normale centrée réduite,
                random = w.testWrapper(m);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex);
                ViewData["Exception"] = ex;
                return View("Error");
            }

            ViewData["Random"] = random;
            String dataChart = "[[1268179200000, 32.12],[1268265600000, 32.21],[1488326400000, 139.79],[1488412800000, 138.96],[1488499200000, 139.78],[1488758400000, 139.34],[1488844800000, 139.52]]";
            ViewData["dataChart"] = dataChart;
            return View("Test");
        }
    }
}