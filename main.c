#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kana.h"
#include "var.h"

#ifdef KPATHSEA
#include "kp.h"
#endif

char *styfile,*idxfile[256],indfile[256],*dicfile,logfile[256];

#ifdef KPATHSEA
/* default paths */
#ifndef DEFAULT_INDEXSTYLES
#define DEFAULT_INDEXSTYLES "."
#endif
#ifndef DEFAULT_INDEXDICTS
#define DEFAULT_INDEXDICTS "."
#endif
KpathseaSupportInfo kp_ist,kp_dict;
#endif

#define VERSION "version 2.6b [24-Mar-2005]"

main(argc,argv)
int argc;
char **argv;
{
	int i,j,k,cc,dd,startpagenum,ecount=0;
	char buff[256],*envbuff;
	FILE *fp;
	char *convcode();

#ifdef KPATHSEA
	KP_init(argv[0]);
	kp_ist.var_name = "INDEXSTYLE";
	kp_ist.path = DEFAULT_INDEXSTYLES; /* default path. */
	kp_ist.suffix = "ist";
	KP_entry_filetype(&kp_ist);
	kp_dict.var_name = "INDEXDICTIONARY";
	kp_dict.path = DEFAULT_INDEXDICTS; /* default path */
	kp_dict.suffix = "dict";
	KP_entry_filetype(&kp_dict);
#endif

/*   check options   */

	for (i=1,j=0;i<argc;i++) {
		if ((argv[i][0]=='-')&&(strlen(argv[i])>=2)) {
			switch (argv[i][1]) {
			case 'c':
				bcomp=1;
				break;

			case 'd':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					i++;
					dicfile=malloc(strlen(argv[i])+1);
					strcpy(dicfile,argv[i]);
				}
				else {
					dicfile=malloc(strlen(&argv[i][2])+1);
					strcpy(dicfile,&argv[i][2]);
				}
				break;

			case 'f':
				force=1;
				break;

			case 'g':
				gflg=1;
				break;

			case 'i':
				fsti=1;
				break;

			case 'l':
				lorder=1;
				break;

			case 'o':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					strcpy(indfile,argv[++i]);
				}
				else {
					strcpy(indfile,&argv[i][2]);
				}
				break;

			case 'p':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					i++;
					if (strcmp(argv[i],"any")==0) fpage=2;
					else if (strcmp(argv[i],"odd")==0) fpage=3;
					else if (strcmp(argv[i],"even")==0) fpage=4;
					else {
						fpage=1;
						startpagenum=atoi(argv[i]);
					}
				}
				else {
					if (strcmp(&argv[i][2],"any")==0) fpage=2;
					else if (strcmp(&argv[i][2],"odd")==0) fpage=3;
					else if (strcmp(&argv[i][2],"even")==0) fpage=4;
					else {
						fpage=1;
						startpagenum=atoi(&argv[i][2]);
					}
				}
				break;

			case 'q':
				verb=0;
				break;

			case 't':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					strcpy(logfile,argv[++i]);
				}
				else {
					strcpy(logfile,&argv[i][2]);
				}
				break;

			case 'r':
				prange=0;
				break;

			case 's':
				if ((argv[i][2]=='\0')&&(i+1<argc)) {
					i++;
					styfile=malloc(strlen(argv[i])+1);
					strcpy(styfile,argv[i]);
				}
				else {
					styfile=malloc(strlen(&argv[i][2])+1);
					strcpy(styfile,&argv[i][2]);
				}
				break;

			case 'v':
				debug=1;
				break;

			case 'E':
				kanji=Euc;
				break;

			case 'J':
				kanji=Jis;
				break;

			case 'S':
				kanji=Sjis;
				break;

			default:
				fprintf(stderr,"mendex - Japanese index processor, %s.\n",VERSION);
				fprintf(stderr," Copyright 1995-2005 ASCII Corp.(www-ptex@ascii.co.jp)\n");
				fprintf(stderr,"usage:\n");
				fprintf(stderr,"%% mendex [-ilqrcg] [-s sty] [-d dic] [-o ind] [-t log] [-p no] [idx0 idx1 ...]\n");
				fprintf(stderr,"options:\n");
				fprintf(stderr,"-i      use stdin as the input file.\n");
				fprintf(stderr,"-l      use letter ordering.\n");
				fprintf(stderr,"-q      quiet mode.\n");
				fprintf(stderr,"-r      disable implicit page formation.\n");
				fprintf(stderr,"-c      compress blanks. (ignore leading and trailing blanks.)\n");
				fprintf(stderr,"-g      make Japanese index head <%s>.\n",convcode(AKASATANA));
				fprintf(stderr,"-f      force to output kanji.\n");
				fprintf(stderr,"-s sty  take sty as style file.\n");
				fprintf(stderr,"-d dic  take dic as dictionary file.\n");
				fprintf(stderr,"-o ind  take ind as the output index file.\n");
				fprintf(stderr,"-t log  take log as the error log file.\n");
				fprintf(stderr,"-p no   set the starting page number of index.\n");
				fprintf(stderr,"-E      EUC mode.\n");
				fprintf(stderr,"-J      JIS mode.\n");
				fprintf(stderr,"-S      ShiftJIS mode.\n");
				fprintf(stderr,"idx...  input files.\n");
				exit(0);
				break;
			}
		}
		else {
			cc=strlen(argv[i]);
			if (cc<4) cc+=4;
			else if (strcmp(&argv[i][cc-4],".idx")) cc+=4;
			idxfile[j]=malloc(cc+1);
			strcpy(idxfile[j++],argv[i]);
		}
	}
	idxcount=j+fsti;

/*   check option errors   */

	if (idxcount==0) idxcount=fsti=1;

	if (styfile==NULL) {
#ifdef KPATHSEA
#ifdef KPATHSEA3
		envbuff=KP_get_value("INDEXDEFAULTSTYLE",NULL);
#else
		envbuff=KP_get_value("INDEXDEFAULTSTYLE","INDEXDEFAULTSTYLE",NULL);
#endif
#else
		envbuff=getenv("INDEXDEFAULTSTYLE");
#endif
		if (envbuff!=NULL) {
			styfile=malloc(strlen(envbuff)+1);
			strcpy(styfile,envbuff);
		}
	}

	if (styfile!=NULL) styread(styfile);

	if ((indfile[0]=='\0')&&(idxcount-fsti>0)) {
		for (i=strlen(idxfile[0]);i>=0;i--) {
			if (idxfile[0][i]=='.') {
				strncpy(indfile,idxfile[0],i);
				sprintf(&indfile[i],".ind");
				break;
			}
		}
		if (i==-1) sprintf(indfile,"%s.ind",idxfile[0]);
	}

	if (logfile[0]=='\0') {
		if (idxcount-fsti>0) {
			for (i=strlen(idxfile[0]);i>=0;i--) {
				if (idxfile[0][i]=='.') {
					strncpy(logfile,idxfile[0],i);
					sprintf(&logfile[i],".ilg");
					break;
				}
			}
			if (i==-1) sprintf(logfile,"%s.ilg",idxfile[0]);
			efp=fopen(logfile,"w");
		}
		else {
			efp=stderr;
			strcpy(logfile,"stderr");
		}
	}
	else efp=fopen(logfile,"w");

	if (strcmp(argv[0],"makeindex")==0) {
		if (verb!=0) fprintf(stderr,"This is Not `MAKEINDEX\', But `MENDEX\' %s.\n",VERSION);
	}
	else {
		if (verb!=0) fprintf(stderr,"This is mendex %s.\n",VERSION);
	}
	if (efp!=stderr) fprintf(efp,"This is mendex %s.\n",VERSION);

/*   init kanatable   */

	initkanatable();

/*   read dictionary   */

	ecount+=dicread(dicfile);

	switch (letter_head) {
	case 0:
	case 1:
		if (gflg==1) {
			strcpy(atama,akasatana);
		}
		else {
			strcpy(atama,aiueo);
		}
		break;

	case 2:
		if (gflg==1) {
			strcpy(atama,AKASATANA);
		}
		else {
			strcpy(atama,AIUEO);
		}
		break;

	default:
		break;
	}

/*   read idx file   */

	lines=0;
	ecount=0;
	ind=(struct index *)malloc(sizeof(struct index));

	for (i=0;i<idxcount-fsti;i++) {
		ecount+=idxread(idxfile[i],lines);
	}
	if (fsti==1) {
		ecount+=idxread(NULL,lines);
	}
	if (verb!=0) {
		fprintf(stderr,"%d entries accepted, %d rejected.\n",acc,reject);
	}
	if (efp!=stderr) {
		fprintf(efp,"%d entries accepted, %d rejected.\n",acc,reject);
	}

	if (ecount!=0) {
		if (verb!=0) {
			fprintf(stderr,"%d errors, written in %s.\n",ecount,logfile);
		}
		if (efp!=stderr) {
			fprintf(efp,"%d errors.\n",ecount);
		}
		lines=0;
	}
	if (lines==0) {
		if (verb!=0) {
			fprintf(stderr,"Nothing written in output file.\n");
		}
		if (efp!=stderr) {
			fprintf(efp,"Nothing written in output file.\n");
			fclose(efp);
		}
		exit(-1);
	}

/*   sort index   */

	if (verb!=0) fprintf(stderr,"Sorting index.");
	if (efp!=stderr) fprintf(efp,"Sorting index.");

	scount=0;
	wsort(ind,lines);

	if (verb!=0) {
		fprintf(stderr,"...done(%d comparisons).\n",scount);
	}
	if (efp!=stderr) {
		fprintf(efp,"...done(%d comparisons).\n",scount);
	}

/*   sort pages   */

	if (verb!=0) fprintf(stderr,"Sorting pages.");
	if (efp!=stderr) fprintf(efp,"Sorting pages.");

	scount=0;
	pagesort(ind,lines);


	if (verb!=0) {
		fprintf(stderr,"...done(%d comparisons).\n",scount);
	}
	if (efp!=stderr) {
		fprintf(efp,"...done(%d comparisons).\n",scount);
	}

/*   get last page   */

	if ((fpage>1)&&(idxcount-fsti>0)) cc=lastpage(idxfile[0]);

	switch (fpage) {
	case 2:
		startpagenum=cc+1;
		break;

	case 3:
		if ((cc+1)%2==0) startpagenum=cc+2;
		else startpagenum=cc+1;
		break;

	case 4:
		if ((cc+1)%2==1) startpagenum=cc+2;
		else startpagenum=cc+1;
		break;
		
	default:
		break;
	}

/*   write indfile   */

	if (verb!=0) fprintf(stderr,"Making index file.");
	if (efp!=stderr) fprintf(efp,"Making index file.");

	indwrite(indfile,ind,startpagenum);

	if (verb!=0) fprintf(stderr,"...done.\n");
	if (efp!=stderr) fprintf(efp,"...done.\n");

	if (idxcount-fsti==0) strcpy(indfile,"stdout");

	if (verb!=0) {
		fprintf(stderr,"%d warnings, written in %s.\n",warn,logfile);
		fprintf(stderr,"Output written in %s.\n",indfile);
	}
	if (efp!=stderr) {
		fprintf(efp,"%d warnings, written in %s.\n",warn,logfile);
		fprintf(efp,"Output written in %s.\n",indfile);
		fclose(efp);
	}

	exit(0);
}

Fprintf(buff)
char *buff;
{
	if (debug) fprintf(stderr,"CHECK:%s",buff);
}
