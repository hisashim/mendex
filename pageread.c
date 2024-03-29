#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lastpage(filename)
/*   checking last page   */
char *filename;
{
	int i,j,cc,num= -1;
	char logfile[32],numbuff[16];
	FILE *fp;

	for (i=strlen(filename);i>=0;i--) {
		if (filename[i]=='.') {
			strncpy(logfile,filename,i);
			sprintf(&logfile[i],".log");
			break;
		}
	}
	if (i==-1) sprintf(logfile,"%s.log",filename);

	fp=fopen(logfile,"r");
	if (fp==NULL) {
		fprintf(stderr,"No log file, %s.\n",logfile);
		exit(0);
	}

	while (1) {
		cc=fgetc(fp);
		if (cc==EOF) break;
		else if (cc=='[') {
			for (i=0;;i++) {
				cc=fgetc(fp);
				if (((i==0)&&(cc=='-'))||((cc>='0')&&(cc<='9'))) numbuff[i]=cc;
				else {
					numbuff[i]='\0';
					break;
				}
			}
			if (strlen(numbuff)>0) num=atoi(numbuff);
		}
	}

	fclose(fp);

	return num;
}
