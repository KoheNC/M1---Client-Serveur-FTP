/*****************************************************************************
 Fonction pour formater un nombre en toute base
 ==============================================

 Le nombre est placé à la fin du buffer fourni, car c'est ainsi que se fait
 naturellement la conversion. L'adresse du nombre dans le buffer est retournée.

 +------+-------+
 |      |123456*|
 +------+-------+
         ^
         |
   pointeur retourné

 *****************************************************************************/

#include <stdlib.h>

char * FormatEntier(
 unsigned long uNombre,	/*[in] Nombre à convertir */
 unsigned int const uBase,	/*[in] Base */
 int const bNombreSigne,	/*[in] TRUE si le nombre est signé */
 char * const szBuffer,	/*[out] Buffer de caractères */
 size_t nCchBuffer	/*[in] Taille du buffer, en caractères */
 )
{
int bNegatif = 0;
char * pt_carac = szBuffer + nCchBuffer;

/* Le buffer doit faire au moins 1 caractère: Le carac. nul. */
if(nCchBuffer < 1)
	return 0;

/* Place le zéro terminal */
--pt_carac, --nCchBuffer;
*pt_carac = '\0';

/* Ajuste le nombre s'il est négatif */
if(bNombreSigne)
	{
	long nNombre = uNombre;
	if(nNombre < 0)
		{
		/* Prend la valeur absolue du nombre et mémorise le fait
			qu'il faille le précéder d'un '-' */
		nNombre = -nNombre;
		uNombre = nNombre;
		bNegatif = 1;
		}/* if négatif */
	}/* if signé */

/* Convertit le nombre */
do	{
	unsigned int const uChiffre = uNombre % uBase;

	if(nCchBuffer == 0)
		break;

	uNombre /= uBase;

	/* Ajoute le chiffre à la fin */
	--pt_carac, --nCchBuffer;
	*pt_carac = (char)( (uChiffre < 10) ? ('0'+uChiffre) : ('A'+(uChiffre-10)) );
	} while(uNombre);

/* Rajoute le signe - si négatif et s'il y a la place */
if(bNegatif)
	{
	if(nCchBuffer==0)
		{
		/* Plus de place */
		}
	else
		{
		--pt_carac, --nCchBuffer;
		*pt_carac = '-';
		}
	}
return pt_carac;
}
