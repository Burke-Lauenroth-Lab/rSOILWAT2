/********************************************************/
/********************************************************/
/*	Source file: Veg_Prod.c
Type: module
Application: SOILWAT - soilwater dynamics simulator
Purpose: Read / write and otherwise manage the model's
vegetation production parameter information.
*/
/********************************************************/
/********************************************************/

/* =================================================== */
/* =================================================== */
/*                INCLUDES / DEFINES                   */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SOILWAT2/generic.h"
#include "SOILWAT2/filefuncs.h"
#include "SOILWAT2/Times.h"
#include "SOILWAT2/myMemory.h"

#include "SOILWAT2/SW_Defines.h"
#include "SOILWAT2/SW_Files.h"

#include "SOILWAT2/SW_VegProd.h"
#include "rSW_VegProd.h"

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>


/* =================================================== */
/*                  Global Variables                   */
/* --------------------------------------------------- */
extern Bool EchoInits;
extern SW_VEGPROD SW_VegProd;

/* =================================================== */
/*                Module-Level Variables               */
/* --------------------------------------------------- */
static char *MyFileName;

/* =================================================== */
/* =================================================== */
/*             Private Function Definitions            */
/* --------------------------------------------------- */


/* =================================================== */
/* =================================================== */
/*             Public Function Definitions             */
/* --------------------------------------------------- */

char *cVegProd_names[] = { "Composition", "Albedo", "CanopyHeight",
	"VegetationInterceptionParameters", "LitterInterceptionParameters",
	"EsTpartitioning_param", "Es_param_limit", "Shade", "HydraulicRedistribution_use",
	"HydraulicRedistribution", "CriticalSoilWaterPotential", "MonthlyVeg",
	"CO2Coefficients" };

char *cMonths[] = { "January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December" };


SEXP onGet_SW_VPD() {
	int i;
	SW_VEGPROD *v = &SW_VegProd;
	SEXP swProd;
	SEXP VegProd;

	SEXP VegComp, VegComp_names, vegtype_names, col_names;
	SEXP Albedo;

	SEXP Canopy, Canopy_names, Canopy_names_x;
	char *cCanopy_names_x[] = { "xinflec", "yinflec", "range", "slope", "height_cm" };
	RealD *p_Canopy;

	SEXP VegInterception, VegInterception_names, VegInterception_names_x;
	char *cVegInterception_x[] = { "kSmax", "kdead" };
	RealD *p_VegInterception;

	SEXP LitterInterception, LitterInterception_names, LitterInterception_names_x;
	char *cLitterInterception_x[] = { "kSmax" };
	RealD *p_LitterInterception;

	SEXP EsTpartitioning_param;
	SEXP Es_param_limit;

	SEXP Shade, Shade_names, Shade_names_x;
	char *cShade_names_x[] = { "ShadeScale", "ShadeMaximalDeadBiomass", "tanfuncXinflec", "yinflec", "range", "slope" };
	RealD *p_Shade;

	SEXP Hydraulic_flag;//"Flag"
	SEXP Hydraulic, Hydraulic_names, Hydraulic_names_x;
	RealD *p_Hydraulic;
	char *cHydraulic_names[] = { "MaxCondRoot", "SoilWaterPotential50", "ShapeCond" };

	SEXP CSWP;

	SEXP MonthlyVeg;
	SEXP Grasslands, Grasslands_names;
	SEXP Shrublands, Shrublands_names;
	SEXP Forest, Forest_names;
	SEXP Forb, Forb_names;

	char *cvegtype_names[] = { "Grasses", "Shrubs", "Trees", "Forbs" };
	PROTECT(vegtype_names = allocVector(STRSXP, NVEGTYPES));
	for (i = 0; i < NVEGTYPES; i++)
		SET_STRING_ELT(vegtype_names, i, mkChar(cvegtype_names[i]));

	/* CO2 */
	// Initialize variables
	SEXP CO2Coefficients, CO2_names, CO2_col_names;
	RealD *p_CO2Coefficients;

	// Create row and column names
	char *cCO2_col_names[] = { "Biomass Coeff1", "Biomass Coeff2", "WUE Coeff1", "WUE Coeff2" };
	PROTECT(CO2_col_names = allocVector(STRSXP, 4));
	for (i = 0; i < 4; i++)
		SET_STRING_ELT(CO2_col_names, i, mkChar(cCO2_col_names[i]));

	// Create matrix containing the multipliers
	PROTECT(CO2Coefficients = allocMatrix(REALSXP, NVEGTYPES, 4));
	p_CO2Coefficients = REAL(CO2Coefficients);
	p_CO2Coefficients[0] = v->veg[SW_GRASS].co2_bio_coeff1;
	p_CO2Coefficients[1] = v->veg[SW_SHRUB].co2_bio_coeff1;
	p_CO2Coefficients[2] = v->veg[SW_TREES].co2_bio_coeff1;
	p_CO2Coefficients[3] = v->veg[SW_FORBS].co2_bio_coeff1;
	p_CO2Coefficients[4] = v->veg[SW_GRASS].co2_bio_coeff2;
	p_CO2Coefficients[5] = v->veg[SW_SHRUB].co2_bio_coeff2;
	p_CO2Coefficients[6] = v->veg[SW_TREES].co2_bio_coeff2;
	p_CO2Coefficients[7] = v->veg[SW_FORBS].co2_bio_coeff2;
	p_CO2Coefficients[8] = v->veg[SW_GRASS].co2_wue_coeff1;
	p_CO2Coefficients[9] = v->veg[SW_SHRUB].co2_wue_coeff1;
	p_CO2Coefficients[10] = v->veg[SW_TREES].co2_wue_coeff1;
	p_CO2Coefficients[11] = v->veg[SW_FORBS].co2_wue_coeff1;
	p_CO2Coefficients[12] = v->veg[SW_GRASS].co2_wue_coeff2;
	p_CO2Coefficients[13] = v->veg[SW_SHRUB].co2_wue_coeff2;
	p_CO2Coefficients[14] = v->veg[SW_TREES].co2_wue_coeff2;
	p_CO2Coefficients[15] = v->veg[SW_FORBS].co2_wue_coeff2;

	// Integrate values with names
	PROTECT(CO2_names = allocVector(VECSXP, 2));
	SET_VECTOR_ELT(CO2_names, 1, CO2_col_names);
	SET_VECTOR_ELT(CO2_names, 0, vegtype_names);
	setAttrib(CO2Coefficients, R_DimNamesSymbol, CO2_names);

	RealD *p_Grasslands, *p_Shrublands, *p_Forest, *p_Forb;
	SEXP MonthlyVeg_Column_names, MonthlyVeg_Row_names;
	char *cMonthlyVeg_Column_names[] = { "Litter", "Biomass", "Live_pct", "LAI_conv" };

	PROTECT(swProd = MAKE_CLASS("swProd"));
	PROTECT(VegProd = NEW_OBJECT(swProd));

	PROTECT(VegComp = allocVector(REALSXP, NVEGTYPES + 1));
	REAL(VegComp)[0] = v->veg[SW_GRASS].cov.fCover; //Grass
	REAL(VegComp)[1] = v->veg[SW_SHRUB].cov.fCover; //Shrub
	REAL(VegComp)[2] = v->veg[SW_TREES].cov.fCover; //Tree
	REAL(VegComp)[3] = v->veg[SW_FORBS].cov.fCover; //forb
	REAL(VegComp)[4] = v->bare_cov.fCover; //Bare Ground

	PROTECT(VegComp_names = allocVector(STRSXP, NVEGTYPES + 1));
	SET_STRING_ELT(VegComp_names, 0, mkChar("Grasses"));
	SET_STRING_ELT(VegComp_names, 1, mkChar("Shrubs"));
	SET_STRING_ELT(VegComp_names, 2, mkChar("Trees"));
	SET_STRING_ELT(VegComp_names, 3, mkChar("Forbs"));
	SET_STRING_ELT(VegComp_names, 4, mkChar("Bare Ground"));
	setAttrib(VegComp, R_NamesSymbol, VegComp_names);

	PROTECT(Albedo = allocVector(REALSXP, NVEGTYPES + 1));
	REAL(Albedo)[0] = v->veg[SW_GRASS].cov.albedo; //Grass
	REAL(Albedo)[1] = v->veg[SW_SHRUB].cov.albedo; //Shrub
	REAL(Albedo)[2] = v->veg[SW_TREES].cov.albedo; //Tree
	REAL(Albedo)[3] = v->veg[SW_FORBS].cov.albedo; //forb
	REAL(Albedo)[4] = v->bare_cov.albedo; //bare ground
	setAttrib(Albedo, R_NamesSymbol, VegComp_names);

	PROTECT(Canopy = allocMatrix(REALSXP, 5, NVEGTYPES));
	p_Canopy = REAL(Canopy);
	p_Canopy[0] = v->veg[SW_GRASS].cnpy.xinflec;
	p_Canopy[1] = v->veg[SW_GRASS].cnpy.yinflec;
	p_Canopy[2] = v->veg[SW_GRASS].cnpy.range;
	p_Canopy[3] = v->veg[SW_GRASS].cnpy.slope;
	p_Canopy[4] = v->veg[SW_GRASS].canopy_height_constant;
	p_Canopy[5] = v->veg[SW_SHRUB].cnpy.xinflec;
	p_Canopy[6] = v->veg[SW_SHRUB].cnpy.yinflec;
	p_Canopy[7] = v->veg[SW_SHRUB].cnpy.range;
	p_Canopy[8] = v->veg[SW_SHRUB].cnpy.slope;
	p_Canopy[9] = v->veg[SW_SHRUB].canopy_height_constant;
	p_Canopy[10] = v->veg[SW_TREES].cnpy.xinflec;
	p_Canopy[11] = v->veg[SW_TREES].cnpy.yinflec;
	p_Canopy[12] = v->veg[SW_TREES].cnpy.range;
	p_Canopy[13] = v->veg[SW_TREES].cnpy.slope;
	p_Canopy[14] = v->veg[SW_TREES].canopy_height_constant;
	p_Canopy[15] = v->veg[SW_FORBS].cnpy.xinflec;
	p_Canopy[16] = v->veg[SW_FORBS].cnpy.yinflec;
	p_Canopy[17] = v->veg[SW_FORBS].cnpy.range;
	p_Canopy[18] = v->veg[SW_FORBS].cnpy.slope;
	p_Canopy[19] = v->veg[SW_FORBS].canopy_height_constant;
	PROTECT(Canopy_names = allocVector(VECSXP, 2));
	PROTECT(Canopy_names_x = allocVector(STRSXP, 5));
	for (i = 0; i < 5; i++)
		SET_STRING_ELT(Canopy_names_x, i, mkChar(cCanopy_names_x[i]));
	SET_VECTOR_ELT(Canopy_names, 0, Canopy_names_x);
	SET_VECTOR_ELT(Canopy_names, 1, vegtype_names);
	setAttrib(Canopy, R_DimNamesSymbol, Canopy_names);

	PROTECT(VegInterception = allocMatrix(REALSXP, 2, NVEGTYPES));
	p_VegInterception = REAL(VegInterception);
	p_VegInterception[0] = v->veg[SW_GRASS].veg_kSmax;
	p_VegInterception[1] = v->veg[SW_GRASS].veg_kdead;
	p_VegInterception[2] = v->veg[SW_SHRUB].veg_kSmax;
	p_VegInterception[3] = v->veg[SW_SHRUB].veg_kdead;
	p_VegInterception[4] = v->veg[SW_TREES].veg_kSmax;
	p_VegInterception[5] = v->veg[SW_TREES].veg_kdead;
	p_VegInterception[6] = v->veg[SW_FORBS].veg_kSmax;
	p_VegInterception[7] = v->veg[SW_FORBS].veg_kdead;
	PROTECT(VegInterception_names = allocVector(VECSXP, 2));
	PROTECT(VegInterception_names_x = allocVector(STRSXP, 2));
	for (i = 0; i < 2; i++)
		SET_STRING_ELT(VegInterception_names_x, i, mkChar(cVegInterception_x[i]));
	SET_VECTOR_ELT(VegInterception_names, 0, VegInterception_names_x);
	SET_VECTOR_ELT(VegInterception_names, 1, vegtype_names);
	setAttrib(VegInterception, R_DimNamesSymbol, VegInterception_names);

	PROTECT(LitterInterception = allocMatrix(REALSXP, 1, NVEGTYPES));
	p_LitterInterception = REAL(LitterInterception);
	p_LitterInterception[0] = v->veg[SW_GRASS].lit_kSmax;
	p_LitterInterception[1] = v->veg[SW_SHRUB].lit_kSmax;
	p_LitterInterception[2] = v->veg[SW_TREES].lit_kSmax;
	p_LitterInterception[3] = v->veg[SW_FORBS].lit_kSmax;
	PROTECT(LitterInterception_names = allocVector(VECSXP, 2));
	PROTECT(LitterInterception_names_x = allocVector(STRSXP, 1));
	for (i = 0; i < 1; i++)
		SET_STRING_ELT(LitterInterception_names_x, i, mkChar(cLitterInterception_x[i]));
	SET_VECTOR_ELT(LitterInterception_names, 0, LitterInterception_names_x);
	SET_VECTOR_ELT(LitterInterception_names, 1, vegtype_names);
	setAttrib(LitterInterception, R_DimNamesSymbol, LitterInterception_names);

	PROTECT(EsTpartitioning_param = allocVector(REALSXP, NVEGTYPES));
	REAL(EsTpartitioning_param)[0] = v->veg[SW_GRASS].EsTpartitioning_param; //Grass
	REAL(EsTpartitioning_param)[1] = v->veg[SW_SHRUB].EsTpartitioning_param; //Shrub
	REAL(EsTpartitioning_param)[2] = v->veg[SW_TREES].EsTpartitioning_param; //Tree
	REAL(EsTpartitioning_param)[3] = v->veg[SW_FORBS].EsTpartitioning_param; //forb
	setAttrib(EsTpartitioning_param, R_NamesSymbol, vegtype_names);

	PROTECT(Es_param_limit = allocVector(REALSXP, NVEGTYPES));
	REAL(Es_param_limit)[0] = v->veg[SW_GRASS].Es_param_limit; //Grass
	REAL(Es_param_limit)[1] = v->veg[SW_SHRUB].Es_param_limit; //Shrub
	REAL(Es_param_limit)[2] = v->veg[SW_TREES].Es_param_limit; //Tree
	REAL(Es_param_limit)[3] = v->veg[SW_FORBS].Es_param_limit; //forb
	setAttrib(Es_param_limit, R_NamesSymbol, vegtype_names);

	PROTECT(Shade = allocMatrix(REALSXP, 6, NVEGTYPES));
	p_Shade = REAL(Shade);
	p_Shade[0] = v->veg[SW_GRASS].shade_scale;
	p_Shade[1] = v->veg[SW_GRASS].shade_deadmax;
	p_Shade[2] = v->veg[SW_GRASS].tr_shade_effects.xinflec;
	p_Shade[3] = v->veg[SW_GRASS].tr_shade_effects.yinflec;
	p_Shade[4] = v->veg[SW_GRASS].tr_shade_effects.range;
	p_Shade[5] = v->veg[SW_GRASS].tr_shade_effects.slope;
	p_Shade[6] = v->veg[SW_SHRUB].shade_scale;
	p_Shade[7] = v->veg[SW_SHRUB].shade_deadmax;
	p_Shade[8] = v->veg[SW_SHRUB].tr_shade_effects.xinflec;
	p_Shade[9] = v->veg[SW_SHRUB].tr_shade_effects.yinflec;
	p_Shade[10] = v->veg[SW_SHRUB].tr_shade_effects.range;
	p_Shade[11] = v->veg[SW_SHRUB].tr_shade_effects.slope;
	p_Shade[12] = v->veg[SW_TREES].shade_scale;
	p_Shade[13] = v->veg[SW_TREES].shade_deadmax;
	p_Shade[14] = v->veg[SW_TREES].tr_shade_effects.xinflec;
	p_Shade[15] = v->veg[SW_TREES].tr_shade_effects.yinflec;
	p_Shade[16] = v->veg[SW_TREES].tr_shade_effects.range;
	p_Shade[17] = v->veg[SW_TREES].tr_shade_effects.slope;
	p_Shade[18] = v->veg[SW_FORBS].shade_scale;
	p_Shade[19] = v->veg[SW_FORBS].shade_deadmax;
	p_Shade[20] = v->veg[SW_FORBS].tr_shade_effects.xinflec;
	p_Shade[21] = v->veg[SW_FORBS].tr_shade_effects.yinflec;
	p_Shade[22] = v->veg[SW_FORBS].tr_shade_effects.range;
	p_Shade[23] = v->veg[SW_FORBS].tr_shade_effects.slope;
	PROTECT(Shade_names = allocVector(VECSXP, 2));
	PROTECT(Shade_names_x = allocVector(STRSXP, 6));
	for (i = 0; i < 6; i++)
		SET_STRING_ELT(Shade_names_x, i, mkChar(cShade_names_x[i]));
	SET_VECTOR_ELT(Shade_names, 0, Shade_names_x);
	SET_VECTOR_ELT(Shade_names, 1, vegtype_names);
	setAttrib(Shade, R_DimNamesSymbol, Shade_names);

	PROTECT(Hydraulic_flag = allocVector(LGLSXP, NVEGTYPES));
	LOGICAL_POINTER(Hydraulic_flag)[0] = v->veg[SW_GRASS].flagHydraulicRedistribution; //Grass
	LOGICAL_POINTER(Hydraulic_flag)[1] = v->veg[SW_SHRUB].flagHydraulicRedistribution; //Shrub
	LOGICAL_POINTER(Hydraulic_flag)[2] = v->veg[SW_TREES].flagHydraulicRedistribution; //Tree
	LOGICAL_POINTER(Hydraulic_flag)[3] = v->veg[SW_FORBS].flagHydraulicRedistribution; //forb
	setAttrib(Hydraulic_flag, R_NamesSymbol, vegtype_names);

	PROTECT(Hydraulic = allocMatrix(REALSXP, 3, NVEGTYPES));
	p_Hydraulic = REAL(Hydraulic);
	p_Hydraulic[0] = v->veg[SW_GRASS].maxCondroot;
	p_Hydraulic[1] = v->veg[SW_GRASS].swpMatric50;
	p_Hydraulic[2] = v->veg[SW_GRASS].shapeCond;
	p_Hydraulic[3] = v->veg[SW_SHRUB].maxCondroot;
	p_Hydraulic[4] = v->veg[SW_SHRUB].swpMatric50;
	p_Hydraulic[5] = v->veg[SW_SHRUB].shapeCond;
	p_Hydraulic[6] = v->veg[SW_TREES].maxCondroot;
	p_Hydraulic[7] = v->veg[SW_TREES].swpMatric50;
	p_Hydraulic[8] = v->veg[SW_TREES].shapeCond;
	p_Hydraulic[9] = v->veg[SW_FORBS].maxCondroot;
	p_Hydraulic[10] = v->veg[SW_FORBS].swpMatric50;
	p_Hydraulic[11] = v->veg[SW_FORBS].shapeCond;
	PROTECT(Hydraulic_names = allocVector(VECSXP, 2));
	PROTECT(Hydraulic_names_x = allocVector(STRSXP, 3));
	for (i = 0; i < 3; i++) {
		SET_STRING_ELT(Hydraulic_names_x, i, mkChar(cHydraulic_names[i]));
	}
	SET_VECTOR_ELT(Hydraulic_names, 0, Hydraulic_names_x);
	SET_VECTOR_ELT(Hydraulic_names, 1, vegtype_names);
	setAttrib(Hydraulic, R_DimNamesSymbol, Hydraulic_names);

	PROTECT(CSWP = allocVector(REALSXP, NVEGTYPES));
	REAL(CSWP)[0] = v->veg[SW_GRASS].SWPcrit / -10; //Grass
	REAL(CSWP)[1] = v->veg[SW_SHRUB].SWPcrit / -10; //Shrub
	REAL(CSWP)[2] = v->veg[SW_TREES].SWPcrit / -10; //Tree
	REAL(CSWP)[3] = v->veg[SW_FORBS].SWPcrit / -10; //Forb
	setAttrib(CSWP, R_NamesSymbol, vegtype_names);

	PROTECT(MonthlyVeg_Column_names = allocVector(STRSXP, 4));
	for (i = 0; i < 4; i++)
		SET_STRING_ELT(MonthlyVeg_Column_names, i, mkChar(cMonthlyVeg_Column_names[i]));
	PROTECT(MonthlyVeg_Row_names = allocVector(STRSXP, 12));
	for (i = 0; i < 12; i++)
		SET_STRING_ELT(MonthlyVeg_Row_names, i, mkChar(cMonths[i]));

	PROTECT(Grasslands = allocMatrix(REALSXP, 12, 4));
	p_Grasslands = REAL(Grasslands);
	for (i = 0; i < 12; i++) {
		p_Grasslands[i + 12 * 0] = v->veg[SW_GRASS].litter[i];
		p_Grasslands[i + 12 * 1] = v->veg[SW_GRASS].biomass[i];
		p_Grasslands[i + 12 * 2] = v->veg[SW_GRASS].pct_live[i];
		p_Grasslands[i + 12 * 3] = v->veg[SW_GRASS].lai_conv[i];
	}
	PROTECT(Grasslands_names = allocVector(VECSXP, 2));
	SET_VECTOR_ELT(Grasslands_names, 0, MonthlyVeg_Row_names);
	SET_VECTOR_ELT(Grasslands_names, 1, MonthlyVeg_Column_names);
	setAttrib(Grasslands, R_DimNamesSymbol, Grasslands_names);

	PROTECT(Shrublands = allocMatrix(REALSXP, 12, 4));
	p_Shrublands = REAL(Shrublands);
	for (i = 0; i < 12; i++) {
		p_Shrublands[i + 12 * 0] = v->veg[SW_SHRUB].litter[i];
		p_Shrublands[i + 12 * 1] = v->veg[SW_SHRUB].biomass[i];
		p_Shrublands[i + 12 * 2] = v->veg[SW_SHRUB].pct_live[i];
		p_Shrublands[i + 12 * 3] = v->veg[SW_SHRUB].lai_conv[i];
	}
	PROTECT(Shrublands_names = allocVector(VECSXP, 2));
	SET_VECTOR_ELT(Shrublands_names, 0, MonthlyVeg_Row_names);
	SET_VECTOR_ELT(Shrublands_names, 1, MonthlyVeg_Column_names);
	setAttrib(Shrublands, R_DimNamesSymbol, Shrublands_names);

	PROTECT(Forest = allocMatrix(REALSXP, 12, 4));
	p_Forest = REAL(Forest);
	for (i = 0; i < 12; i++) {
		p_Forest[i + 12 * 0] = v->veg[SW_TREES].litter[i];
		p_Forest[i + 12 * 1] = v->veg[SW_TREES].biomass[i];
		p_Forest[i + 12 * 2] = v->veg[SW_TREES].pct_live[i];
		p_Forest[i + 12 * 3] = v->veg[SW_TREES].lai_conv[i];
	}
	PROTECT(Forest_names = allocVector(VECSXP, 2));
	SET_VECTOR_ELT(Forest_names, 0, MonthlyVeg_Row_names);
	SET_VECTOR_ELT(Forest_names, 1, MonthlyVeg_Column_names);
	setAttrib(Forest, R_DimNamesSymbol, Forest_names);

	PROTECT(Forb = allocMatrix(REALSXP, 12, 4));
	p_Forb = REAL(Forb);
	for (i = 0; i < 12; i++) {
		p_Forb[i + 12 * 0] = v->veg[SW_FORBS].litter[i];
		p_Forb[i + 12 * 1] = v->veg[SW_FORBS].biomass[i];
		p_Forb[i + 12 * 2] = v->veg[SW_FORBS].pct_live[i];
		p_Forb[i + 12 * 3] = v->veg[SW_FORBS].lai_conv[i];
	}
	PROTECT(Forb_names = allocVector(VECSXP, 2));
	SET_VECTOR_ELT(Forb_names, 0, MonthlyVeg_Row_names);
	SET_VECTOR_ELT(Forb_names, 1, MonthlyVeg_Column_names);
	setAttrib(Forb, R_DimNamesSymbol, Forb_names);

	PROTECT(MonthlyVeg = allocVector(VECSXP, NVEGTYPES));
	SET_VECTOR_ELT(MonthlyVeg, SW_TREES, Forest);
	SET_VECTOR_ELT(MonthlyVeg, SW_SHRUB, Shrublands);
	SET_VECTOR_ELT(MonthlyVeg, SW_FORBS, Forb);
	SET_VECTOR_ELT(MonthlyVeg, SW_GRASS, Grasslands);

	PROTECT(col_names = allocVector(STRSXP, NVEGTYPES));
	SET_STRING_ELT(col_names, 0, mkChar("Trees"));
	SET_STRING_ELT(col_names, 1, mkChar("Shrubs"));
	SET_STRING_ELT(col_names, 2, mkChar("Forbs"));
	SET_STRING_ELT(col_names, 3, mkChar("Grasses"));
	setAttrib(MonthlyVeg, R_NamesSymbol, col_names);

	SET_SLOT(VegProd, install(cVegProd_names[0]), VegComp);
	SET_SLOT(VegProd, install(cVegProd_names[1]), Albedo);
	SET_SLOT(VegProd, install(cVegProd_names[2]), Canopy);
	SET_SLOT(VegProd, install(cVegProd_names[3]), VegInterception);
	SET_SLOT(VegProd, install(cVegProd_names[4]), LitterInterception);
	SET_SLOT(VegProd, install(cVegProd_names[5]), EsTpartitioning_param);
	SET_SLOT(VegProd, install(cVegProd_names[6]), Es_param_limit);
	SET_SLOT(VegProd, install(cVegProd_names[7]), Shade);
	SET_SLOT(VegProd, install(cVegProd_names[8]), Hydraulic_flag);
	SET_SLOT(VegProd, install(cVegProd_names[9]), Hydraulic);
	SET_SLOT(VegProd, install(cVegProd_names[10]), CSWP);
	SET_SLOT(VegProd, install(cVegProd_names[11]), MonthlyVeg);
	SET_SLOT(VegProd, install(cVegProd_names[12]), CO2Coefficients);

	UNPROTECT(40);
	return VegProd;
}

void onSet_SW_VPD(SEXP SW_VPD) {
	int i;
	SW_VEGPROD *v = &SW_VegProd;

	SEXP VegComp;
	SEXP Albedo;
	SEXP Canopy;
	RealD *p_Canopy;
	SEXP VegInterception;
	RealD *p_VegInterception;
	SEXP LitterInterception;
	RealD *p_LitterInterception;
	SEXP EsTpartitioning_param;
	SEXP Es_param_limit;
	SEXP Shade;
	RealD *p_Shade;
	SEXP Hydraulic;
	SEXP Hydraulic_flag;
	SEXP CSWP;
	SEXP MonthlyVeg, Grasslands, Shrublands, Forest, Forb;
	SEXP CO2Coefficients;
	RealD *p_Grasslands, *p_Shrublands, *p_Forest, *p_Forb;

	MyFileName = SW_F_name(eVegProd);

	PROTECT(VegComp = GET_SLOT(SW_VPD, install(cVegProd_names[0])));
	v->veg[SW_GRASS].cov.fCover = REAL(VegComp)[0]; //Grass
	v->veg[SW_SHRUB].cov.fCover = REAL(VegComp)[1]; //Shrub
	v->veg[SW_TREES].cov.fCover = REAL(VegComp)[2]; //Tree
	v->veg[SW_FORBS].cov.fCover = REAL(VegComp)[3]; //Forb
	v->bare_cov.fCover = REAL(VegComp)[4]; //Bare Ground

	PROTECT(Albedo = GET_SLOT(SW_VPD, install(cVegProd_names[1])));
	v->veg[SW_GRASS].cov.albedo = REAL(Albedo)[0]; //Grass
	v->veg[SW_SHRUB].cov.albedo = REAL(Albedo)[1]; //Shrub
	v->veg[SW_TREES].cov.albedo = REAL(Albedo)[2]; //Tree
	v->veg[SW_FORBS].cov.albedo = REAL(Albedo)[3]; //Forb
	v->bare_cov.albedo = REAL(Albedo)[4]; //Bare Ground

	PROTECT(Canopy = GET_SLOT(SW_VPD, install(cVegProd_names[2])));
	p_Canopy = REAL(Canopy);
	v->veg[SW_GRASS].cnpy.xinflec = p_Canopy[0];
	v->veg[SW_GRASS].cnpy.yinflec = p_Canopy[1];
	v->veg[SW_GRASS].cnpy.range = p_Canopy[2];
	v->veg[SW_GRASS].cnpy.slope = p_Canopy[3];
	v->veg[SW_GRASS].canopy_height_constant = p_Canopy[4];
	v->veg[SW_SHRUB].cnpy.xinflec = p_Canopy[5];
	v->veg[SW_SHRUB].cnpy.yinflec = p_Canopy[6];
	v->veg[SW_SHRUB].cnpy.range = p_Canopy[7];
	v->veg[SW_SHRUB].cnpy.slope = p_Canopy[8];
	v->veg[SW_SHRUB].canopy_height_constant = p_Canopy[9];
	v->veg[SW_TREES].cnpy.xinflec = p_Canopy[10];
	v->veg[SW_TREES].cnpy.yinflec = p_Canopy[11];
	v->veg[SW_TREES].cnpy.range = p_Canopy[12];
	v->veg[SW_TREES].cnpy.slope = p_Canopy[13];
	v->veg[SW_TREES].canopy_height_constant = p_Canopy[14];
	v->veg[SW_FORBS].cnpy.xinflec = p_Canopy[15];
	v->veg[SW_FORBS].cnpy.yinflec = p_Canopy[16];
	v->veg[SW_FORBS].cnpy.range = p_Canopy[17];
	v->veg[SW_FORBS].cnpy.slope = p_Canopy[18];
	v->veg[SW_FORBS].canopy_height_constant = p_Canopy[19];

	PROTECT(VegInterception = GET_SLOT(SW_VPD, install(cVegProd_names[3])));
	p_VegInterception = REAL(VegInterception);
	v->veg[SW_GRASS].veg_kSmax = p_VegInterception[0];
	v->veg[SW_GRASS].veg_kdead = p_VegInterception[1];
	v->veg[SW_SHRUB].veg_kSmax = p_VegInterception[2];
	v->veg[SW_SHRUB].veg_kdead = p_VegInterception[3];
	v->veg[SW_TREES].veg_kSmax = p_VegInterception[4];
	v->veg[SW_TREES].veg_kdead = p_VegInterception[5];
	v->veg[SW_FORBS].veg_kSmax = p_VegInterception[6];
	v->veg[SW_FORBS].veg_kdead = p_VegInterception[7];

	PROTECT(LitterInterception = GET_SLOT(SW_VPD, install(cVegProd_names[4])));
	p_LitterInterception = REAL(LitterInterception);
	v->veg[SW_GRASS].lit_kSmax = p_LitterInterception[0];
	v->veg[SW_SHRUB].lit_kSmax = p_LitterInterception[1];
	v->veg[SW_TREES].lit_kSmax = p_LitterInterception[2];
	v->veg[SW_FORBS].lit_kSmax = p_LitterInterception[3];

	PROTECT(EsTpartitioning_param = GET_SLOT(SW_VPD, install(cVegProd_names[5])));
	v->veg[SW_GRASS].EsTpartitioning_param = REAL(EsTpartitioning_param)[0]; //Grass
	v->veg[SW_SHRUB].EsTpartitioning_param = REAL(EsTpartitioning_param)[1]; //Shrub
	v->veg[SW_TREES].EsTpartitioning_param = REAL(EsTpartitioning_param)[2]; //Tree
	v->veg[SW_FORBS].EsTpartitioning_param = REAL(EsTpartitioning_param)[3]; //Forb

	PROTECT(Es_param_limit = GET_SLOT(SW_VPD, install(cVegProd_names[6])));
	v->veg[SW_GRASS].Es_param_limit = REAL(Es_param_limit)[0]; //Grass
	v->veg[SW_SHRUB].Es_param_limit = REAL(Es_param_limit)[1]; //Shrub
	v->veg[SW_TREES].Es_param_limit = REAL(Es_param_limit)[2]; //Tree
	v->veg[SW_FORBS].Es_param_limit = REAL(Es_param_limit)[3]; //Forb

	PROTECT(Shade = GET_SLOT(SW_VPD, install(cVegProd_names[7])));
	p_Shade = REAL(Shade);
	v->veg[SW_GRASS].shade_scale = p_Shade[0];
	v->veg[SW_GRASS].shade_deadmax = p_Shade[1];
	v->veg[SW_GRASS].tr_shade_effects.xinflec = p_Shade[2];
	v->veg[SW_GRASS].tr_shade_effects.yinflec = p_Shade[3];
	v->veg[SW_GRASS].tr_shade_effects.range = p_Shade[4];
	v->veg[SW_GRASS].tr_shade_effects.slope = p_Shade[5];
	v->veg[SW_SHRUB].shade_scale = p_Shade[6];
	v->veg[SW_SHRUB].shade_deadmax = p_Shade[7];
	v->veg[SW_SHRUB].tr_shade_effects.xinflec = p_Shade[8];
	v->veg[SW_SHRUB].tr_shade_effects.yinflec = p_Shade[9];
	v->veg[SW_SHRUB].tr_shade_effects.range = p_Shade[10];
	v->veg[SW_SHRUB].tr_shade_effects.slope = p_Shade[11];
	v->veg[SW_TREES].shade_scale = p_Shade[12];
	v->veg[SW_TREES].shade_deadmax = p_Shade[13];
	v->veg[SW_TREES].tr_shade_effects.xinflec = p_Shade[14];
	v->veg[SW_TREES].tr_shade_effects.yinflec = p_Shade[15];
	v->veg[SW_TREES].tr_shade_effects.range = p_Shade[16];
	v->veg[SW_TREES].tr_shade_effects.slope = p_Shade[17];
	v->veg[SW_FORBS].shade_scale = p_Shade[18];
	v->veg[SW_FORBS].shade_deadmax = p_Shade[19];
	v->veg[SW_FORBS].tr_shade_effects.xinflec = p_Shade[20];
	v->veg[SW_FORBS].tr_shade_effects.yinflec = p_Shade[21];
	v->veg[SW_FORBS].tr_shade_effects.range = p_Shade[22];
	v->veg[SW_FORBS].tr_shade_effects.slope = p_Shade[23];

	PROTECT(Hydraulic_flag = GET_SLOT(SW_VPD, install(cVegProd_names[8])));
	PROTECT(Hydraulic = GET_SLOT(SW_VPD, install(cVegProd_names[9])));
	v->veg[SW_GRASS].flagHydraulicRedistribution = LOGICAL_POINTER(Hydraulic_flag)[0]; //Grass
	v->veg[SW_SHRUB].flagHydraulicRedistribution = LOGICAL_POINTER(Hydraulic_flag)[1]; //Shrub
	v->veg[SW_TREES].flagHydraulicRedistribution = LOGICAL_POINTER(Hydraulic_flag)[2]; //Tree
	v->veg[SW_FORBS].flagHydraulicRedistribution = LOGICAL_POINTER(Hydraulic_flag)[3]; //Forb
	v->veg[SW_GRASS].maxCondroot = REAL(Hydraulic)[0]; //Grass
	v->veg[SW_GRASS].swpMatric50 = REAL(Hydraulic)[1]; //Grass
	v->veg[SW_GRASS].shapeCond = REAL(Hydraulic)[2]; //Grass
	v->veg[SW_SHRUB].maxCondroot = REAL(Hydraulic)[3]; //Shrub
	v->veg[SW_SHRUB].swpMatric50 = REAL(Hydraulic)[4]; //Shrub
	v->veg[SW_SHRUB].shapeCond = REAL(Hydraulic)[5]; //Shrub
	v->veg[SW_TREES].maxCondroot = REAL(Hydraulic)[6]; //Tree
	v->veg[SW_TREES].swpMatric50 = REAL(Hydraulic)[7]; //Tree
	v->veg[SW_TREES].shapeCond = REAL(Hydraulic)[8]; //Tree
	v->veg[SW_FORBS].maxCondroot = REAL(Hydraulic)[9]; //Forb
	v->veg[SW_FORBS].swpMatric50 = REAL(Hydraulic)[10]; //Forb
	v->veg[SW_FORBS].shapeCond = REAL(Hydraulic)[11]; //Forb

	PROTECT(CSWP = GET_SLOT(SW_VPD, install(cVegProd_names[10])));
	v->veg[SW_GRASS].SWPcrit = -10 * REAL(CSWP)[0]; //Grass
	v->veg[SW_SHRUB].SWPcrit = -10 * REAL(CSWP)[1]; //Shrub
	v->veg[SW_TREES].SWPcrit = -10 * REAL(CSWP)[2]; //Tree
	v->veg[SW_FORBS].SWPcrit = -10 * REAL(CSWP)[3]; //Forb

	// getting critSoilWater for use with SWA and get_critical_rank()
	// critSoilWater goes tree, shrub, forb, grass
	SW_VegProd.critSoilWater[0] = REAL(CSWP)[2];
	SW_VegProd.critSoilWater[1] = REAL(CSWP)[1];
	SW_VegProd.critSoilWater[2] = REAL(CSWP)[3];
	SW_VegProd.critSoilWater[3] = REAL(CSWP)[0];

	get_critical_rank();

	PROTECT(MonthlyVeg = GET_SLOT(SW_VPD, install(cVegProd_names[11])));
	PROTECT(Grasslands = VECTOR_ELT(MonthlyVeg, SW_GRASS));
	p_Grasslands = REAL(Grasslands);
	for (i = 0; i < 12; i++) {
		v->veg[SW_GRASS].litter[i] = p_Grasslands[i + 12 * 0];
		v->veg[SW_GRASS].biomass[i] = p_Grasslands[i + 12 * 1];
		v->veg[SW_GRASS].pct_live[i] = p_Grasslands[i + 12 * 2];
		v->veg[SW_GRASS].lai_conv[i] = p_Grasslands[i + 12 * 3];
	}
	PROTECT(Shrublands = VECTOR_ELT(MonthlyVeg, SW_SHRUB));
	p_Shrublands = REAL(Shrublands);
	for (i = 0; i < 12; i++) {
		v->veg[SW_SHRUB].litter[i] = p_Shrublands[i + 12 * 0];
		v->veg[SW_SHRUB].biomass[i] = p_Shrublands[i + 12 * 1];
		v->veg[SW_SHRUB].pct_live[i] = p_Shrublands[i + 12 * 2];
		v->veg[SW_SHRUB].lai_conv[i] = p_Shrublands[i + 12 * 3];
	}
	PROTECT(Forest = VECTOR_ELT(MonthlyVeg, SW_TREES));
	p_Forest = REAL(Forest);
	for (i = 0; i < 12; i++) {
		v->veg[SW_TREES].litter[i] = p_Forest[i + 12 * 0];
		v->veg[SW_TREES].biomass[i] = p_Forest[i + 12 * 1];
		v->veg[SW_TREES].pct_live[i] = p_Forest[i + 12 * 2];
		v->veg[SW_TREES].lai_conv[i] = p_Forest[i + 12 * 3];
	}
	PROTECT(Forb = VECTOR_ELT(MonthlyVeg, SW_FORBS));
	p_Forb = REAL(Forb);
	for (i = 0; i < 12; i++) {
		v->veg[SW_FORBS].litter[i] = p_Forb[i + 12 * 0];
		v->veg[SW_FORBS].biomass[i] = p_Forb[i + 12 * 1];
		v->veg[SW_FORBS].pct_live[i] = p_Forb[i + 12 * 2];
		v->veg[SW_FORBS].lai_conv[i] = p_Forb[i + 12 * 3];
	}

	PROTECT(CO2Coefficients = GET_SLOT(SW_VPD, install(cVegProd_names[12])));
	v->veg[SW_GRASS].co2_bio_coeff1 = REAL(CO2Coefficients)[0];
	v->veg[SW_SHRUB].co2_bio_coeff1 = REAL(CO2Coefficients)[1];
	v->veg[SW_TREES].co2_bio_coeff1 = REAL(CO2Coefficients)[2];
	v->veg[SW_FORBS].co2_bio_coeff1 = REAL(CO2Coefficients)[3];
	v->veg[SW_GRASS].co2_bio_coeff2 = REAL(CO2Coefficients)[4];
	v->veg[SW_SHRUB].co2_bio_coeff2 = REAL(CO2Coefficients)[5];
	v->veg[SW_TREES].co2_bio_coeff2 = REAL(CO2Coefficients)[6];
	v->veg[SW_FORBS].co2_bio_coeff2 = REAL(CO2Coefficients)[7];
	v->veg[SW_GRASS].co2_wue_coeff1 = REAL(CO2Coefficients)[8];
	v->veg[SW_SHRUB].co2_wue_coeff1 = REAL(CO2Coefficients)[9];
	v->veg[SW_TREES].co2_wue_coeff1 = REAL(CO2Coefficients)[10];
	v->veg[SW_FORBS].co2_wue_coeff1 = REAL(CO2Coefficients)[11];
	v->veg[SW_GRASS].co2_wue_coeff2 = REAL(CO2Coefficients)[12];
	v->veg[SW_SHRUB].co2_wue_coeff2 = REAL(CO2Coefficients)[13];
	v->veg[SW_TREES].co2_wue_coeff2 = REAL(CO2Coefficients)[14];
	v->veg[SW_FORBS].co2_wue_coeff2 = REAL(CO2Coefficients)[15];


  SW_VPD_fix_cover();

	if (EchoInits)
		_echo_VegProd();

	UNPROTECT(17);
}
