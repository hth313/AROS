/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc: Common routines for Gadgets
    Lang: english
*/
#include "intuition_intern.h"
#include "gadgets.h"
#include <exec/types.h>
#include <proto/intuition.h>
#include <intuition/intuition.h>
#include <intuition/cghooks.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <proto/graphics.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#define EG(o) ((struct ExtGadget *)o)
#define IM(o) ((struct Image *)o)

/* print the label of a gadget object */
void printgadgetlabel(Class *cl, Object *o, struct gpRender *msg)
{
    struct RastPort *rp = msg->gpr_RPort;
    struct IBox container;
    UWORD *pens = msg->gpr_GInfo->gi_DrInfo->dri_Pens;

    GetGadgetIBox(o, msg->gpr_GInfo, &container);

    switch (EG(o)->Flags & GFLG_LABELMASK)
    {
    case GFLG_LABELITEXT: {
	struct TextExtent te;

	TextExtent(rp, EG(o)->GadgetText->IText,
		   strlen(EG(o)->GadgetText->IText), &te);
        PrintIText(rp,
	    EG(o)->GadgetText,
	    container.Left,
	    container.Top);
	break; }

    case GFLG_LABELSTRING:
        if(EG(o)->GadgetText != NULL)
	{
	    ULONG len = strlen ((STRPTR) EG(o)->GadgetText);

	    if (len > 0UL)
	    {
	        ULONG x;
		ULONG y;

		x = container.Left + (container.Width / 2);
		x -= LabelWidth (rp,
		    (STRPTR)EG(o)->GadgetText, len, IntuitionBase) / 2;
		y = container.Top + (container.Height / 2) +
		    rp->Font->tf_Baseline;
		y -= rp->Font->tf_YSize / 2;
		SetAPen (rp, pens[TEXTPEN]);
		Move (rp, x, y);
		RenderLabel (rp,
		    (STRPTR) EG(o)->GadgetText, len,
		    IntuitionBase);
	    }
	}
	break;

    case GFLG_LABELIMAGE:
        {
            /* center image position, we assume image top and left is 0 */
            ULONG x = container.Left + ((container.Width / 2) -
		(IM(EG(o)->GadgetText)->Width / 2));
            ULONG y = container.Top + ((container.Height / 2) -
	        (IM(EG(o)->GadgetText)->Height / 2));

            DrawImageState(rp,
	        IM(EG(o)->GadgetText),
		x, y,
		((EG(o)->Flags & GFLG_SELECTED) ? IDS_SELECTED : IDS_NORMAL ),
		msg->gpr_GInfo->gi_DrInfo);
        }
        break;
    }
}

/* Calculate the size of the Bounding Box of the gadget */
void CalcBBox (struct Window * window, struct Gadget * gadget,
	struct BBox * bbox)
{
#define ADDREL(flag,field)  ((gadget->Flags & (flag)) ? window->field : 0)

    bbox->Left	 = ADDREL(GFLG_RELRIGHT,Width-1)   + gadget->LeftEdge;
    bbox->Top	 = ADDREL(GFLG_RELBOTTOM,Height-1) + gadget->TopEdge;
    bbox->Width  = ADDREL(GFLG_RELWIDTH,Width)     + gadget->Width;
    bbox->Height = ADDREL(GFLG_RELHEIGHT,Height)   + gadget->Height;
} /* CalcBBox */

/* Figure out the size of the gadget rectangle, taking relative
 * positioning into account.
 */
VOID GetGadgetIBox(Object *o, struct GadgetInfo *gi, struct IBox *ibox)
{
    ibox->Left	 = EG(o)->LeftEdge;
    ibox->Top	 = EG(o)->TopEdge;
    ibox->Width  = EG(o)->Width;
    ibox->Height = EG(o)->Height;

    if (gi)
    {
	if (EG(o)->Flags & GFLG_RELRIGHT)
	    ibox->Left	 += gi->gi_Domain.Width - 1;

	if (EG(o)->Flags & GFLG_RELBOTTOM)
	    ibox->Top	 += gi->gi_Domain.Height - 1;

	if (EG(o)->Flags & GFLG_RELWIDTH)
	    ibox->Width  += gi->gi_Domain.Width;

	if (EG(o)->Flags & GFLG_RELHEIGHT)
	    ibox->Height += gi->gi_Domain.Height;
    }
}

ULONG LabelWidth (struct RastPort * rp, STRPTR label, ULONG len,
		struct IntuitionBase * IntuitionBase)
{
    ULONG totalwidth, uscorewidth;

    totalwidth	= TextLength (rp, label, len);
    uscorewidth = TextLength (rp, "_", 1);

    while (len && *label)
    {
	if (*label == '_')
	    totalwidth -= uscorewidth;

	label ++;
	len --;
    }

    return totalwidth;
}

void RenderLabel (struct RastPort * rp, STRPTR label, ULONG len,
		struct IntuitionBase * IntuitionBase)
{
    ULONG renderlen;
    ULONG uscorewidth;

    while (*label)
    {
	renderlen = 0;

	while (label[renderlen] && label[renderlen] != '_')
	    renderlen ++;

	Text (rp, label, renderlen);

	label += renderlen;

	if (*label) /* '_' ? */
	{
	    WORD cx, cy;

	    label ++; /* Skip */

	    uscorewidth = TextLength (rp, label, 1);

	    cx = rp->cp_x;
	    cy = rp->cp_y;

	    Move (rp, cx, cy+2);
	    Draw (rp, cx+uscorewidth-1, cy+2);
	    Move (rp, cx, cy);
	}
    }
}

