#include <stdio.h>
#include <process.h>
#include <math.h>

/* G = 2.0 oder etwa 10^(3/10) */
#define G 2.0
/* Normalfrequenz 1000 Hz */
#define fn 1000.0
#define PI 3.141592654

main() {
   int i;
   char infilename[80], outfilename[80];
   char datline[128];
   FILE *infptr;
   FILE *outfptr;

   double ampl, ampl_nodb;
   double freq;
   double fmin;
   double fu, fm, fo;
   double a;
   double nb;
   double bandedge;

   unsigned int n;
   unsigned int b=24;

   int x;



   printf("Daten-Dateiname: ");
   scanf("%s", infilename);
   printf("Ausgabe-Dateiname: ");
   scanf("%s", outfilename);
   printf("Terz-Typ (b): ");
   scanf("%u", &b);
   printf("Effektive Rauschbandbreite (>=1) \n"
	  "Rechteck = 1.0\n"
	  "Hanning  = 1.5\n"
	  "Flat-Top = 3.77\n"
	  "Also was [1.0]: ");

   if (scanf("%lf", &nb)!=1) {
      nb = 1.0;
   }

   bandedge = pow(G, 1.0/(2.0*b));
   if ((infptr = fopen(infilename, "rt")) != NULL) {
      printf("\n™ffne %s", infilename);
      i=0;

      fmin = 0.0;
      do {
	 fgets(datline, 128, infptr);
	 if (sscanf(datline, "%u %lf %lf", &n, &freq, &ampl) == 3) {
	    if (freq > 0) {
	       if (i==0) {
		  fmin = freq;
		  /* Berechnen der ersten Mittenfrequenz... */
		  x = (double) (b * log10(fmin / fn) / log10(G));

	       }

	    }
	 }
      } while (fmin == 0.0);
      printf("\nMinimale Frequenz %lf"
	     "\nErstes x: %d\n", fmin, x);
      outfptr = fopen(outfilename, "wt");
      if (outfptr==NULL) {
	 printf("\nKann Datei %s nicht zum schreiben ”ffnen.\n", outfilename);
	 exit(-1);
      }
      fprintf(outfptr, "# b: %u"
		     "\n# Effektive Rauschbandbreite: %4.3lf"
		     "\n# Daten-Datei: %s"
		     "\n#"
		     "\n# Frequenz |  Amplitude (nicht-dB) | Schwinggeschw. | Zusammengesetzt aus n Werten"
		     "\n#",
		     b, nb, infilename);
      do {
	 fm = pow(G, (double) x/b) * fn;
	 fo = bandedge * fm;
	 fu = 1/bandedge * fm;

	 a=0.0;
	 i=0;

	 do {
	    if (sscanf(datline, "%u %lf %lf", &n, &freq, &ampl) == 3) {
	    /* 10 hoch (daten / 20) */
	       ampl_nodb = pow(10.0, ampl/20.0);
	       a = sqrt(a*a + ampl_nodb*ampl_nodb);
	       i++;
	    } else break;
	    /* den n„chsten einlesen, wenn er noch nicht zu gross ist */
	    if ((freq) < fo) {
	       if (fgets(datline, 128, infptr)==0) break;
	    }

	 } while ((freq+fmin) < fo);
	 /* Mit dem Fenster-Korekturfaktor versehen: */
	 a = a * (1.0/sqrt(nb));
	 fprintf(outfptr, "\n%5.6lf %4.3lf %4.3lf %4u", fm, a, a/(2.0*PI*fm), i);
	 x++;
      } while (!feof(infptr));
      printf("\n%u Datens„tze gelesen.", i);
      fclose(outfptr);
      fclose(infptr);
   } else {
      printf("Falscher Dateiname.");
      exit(-1);
   }
   return(0);
}