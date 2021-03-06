/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2014 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "thermalResistanceCoupledBaffleMixedFvPatchScalarField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "mappedPatchBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace compressible
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

thermalResistanceCoupledBaffleMixedFvPatchScalarField::
thermalResistanceCoupledBaffleMixedFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    mixedFvPatchScalarField(p, iF),
    temperatureCoupledBase(patch(), "undefined", "undefined", "undefined-K"),
    TnbrName_("undefined-Tnbr"),
    thicknessLayers_(0),
    kappaLayers_(0),
    contactRes_(0),
    resistance_(0)
{
    this->refValue() = 0.0;
    this->refGrad() = 0.0;
    this->valueFraction() = 1.0;
}


thermalResistanceCoupledBaffleMixedFvPatchScalarField::
thermalResistanceCoupledBaffleMixedFvPatchScalarField
(
    const thermalResistanceCoupledBaffleMixedFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    mixedFvPatchScalarField(ptf, p, iF, mapper),
    temperatureCoupledBase(patch(), ptf),
    TnbrName_(ptf.TnbrName_),
    thicknessLayers_(ptf.thicknessLayers_),
    kappaLayers_(ptf.kappaLayers_),
    contactRes_(ptf.contactRes_),
    resistance_(ptf.resistance_)
{}


thermalResistanceCoupledBaffleMixedFvPatchScalarField::
thermalResistanceCoupledBaffleMixedFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    mixedFvPatchScalarField(p, iF),
    temperatureCoupledBase(patch(), dict),
    TnbrName_(dict.lookup("Tnbr")),
    thicknessLayers_(0),
    kappaLayers_(0),
    contactRes_(0.0),
    resistance_(0.0)
{
    if (!isA<mappedPatchBase>(this->patch().patch()))
    {
        FatalErrorIn
        (
            "thermalResistanceCoupledBaffleMixedFvPatchScalarField::"
            "thermalResistanceCoupledBaffleMixedFvPatchScalarField\n"
            "(\n"
            "    const fvPatch& p,\n"
            "    const DimensionedField<scalar, volMesh>& iF,\n"
            "    const dictionary& dict\n"
            ")\n"
        )   << "\n    patch type '" << p.type()
            << "' not type '" << mappedPatchBase::typeName << "'"
            << "\n    for patch " << p.name()
            << " of field " << dimensionedInternalField().name()
            << " in file " << dimensionedInternalField().objectPath()
            << exit(FatalError);
    }

    if (dict.found("thicknessLayers") && !dict.found("resistance"))
    {
        dict.lookup("thicknessLayers") >> thicknessLayers_;
        dict.lookup("kappaLayers") >> kappaLayers_;

        if (thicknessLayers_.size() > 0)
        {
            forAll (thicknessLayers_, iLayer)
            {
                const scalar l = thicknessLayers_[iLayer];
                if (l > 0.0)
                {
                    contactRes_ += kappaLayers_[iLayer]/l;
                }
            }
        }
    }
    else if (!dict.found("thicknessLayers") && dict.found("resistance"))
    {
        dict.lookup("resistance") >> resistance_;
    }
    else if (dict.found("thicknessLayers") && dict.found("resistance"))
    {
        FatalErrorIn
        (
            "thermalResistanceCoupledBaffleMixedFvPatchScalarField::"
            "thermalResistanceCoupledBaffleMixedFvPatchScalarField\n"
            "(\n"
            " const fvPatch& p,\n"
            " const DimensionedField<scalar, volMesh>& iF,\n"
            " const dictionary& dict\n"
            ")\n"
        ) << "\n patch type '" << p.type()
            << "' either resistance or thicknessLayers can be defined at the same time '"
            << "\n for patch " << p.name()
            << " of field " << dimensionedInternalField().name()
            << " in file " << dimensionedInternalField().objectPath()
            << exit(FatalError);
    }

    fvPatchScalarField::operator=(scalarField("value", dict, p.size()));

    if (dict.found("refValue"))
    {
        // Full restart
        refValue() = scalarField("refValue", dict, p.size());
        refGrad() = scalarField("refGradient", dict, p.size());
        valueFraction() = scalarField("valueFraction", dict, p.size());
    }
    else
    {
        // Start from user entered data. Assume fixedValue.
        refValue() = *this;
        refGrad() = 0.0;
        valueFraction() = 1.0;
    }
}


thermalResistanceCoupledBaffleMixedFvPatchScalarField::
thermalResistanceCoupledBaffleMixedFvPatchScalarField
(
    const thermalResistanceCoupledBaffleMixedFvPatchScalarField& wtcsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    mixedFvPatchScalarField(wtcsf, iF),
    temperatureCoupledBase(patch(), wtcsf),
    TnbrName_(wtcsf.TnbrName_),
    thicknessLayers_(wtcsf.thicknessLayers_),
    kappaLayers_(wtcsf.kappaLayers_),
    contactRes_(wtcsf.contactRes_),
    resistance_(wtcsf.resistance_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void thermalResistanceCoupledBaffleMixedFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    // Since we're inside initEvaluate/evaluate there might be processor
    // comms underway. Change the tag we use.
    int oldTag = UPstream::msgType();
    UPstream::msgType() = oldTag+1;

    // Get the coupling information from the mappedPatchBase
    const mappedPatchBase& mpp =
        refCast<const mappedPatchBase>(patch().patch());
    const polyMesh& nbrMesh = mpp.sampleMesh();
    const label samplePatchI = mpp.samplePolyPatch().index();
    const fvPatch& nbrPatch =
        refCast<const fvMesh>(nbrMesh).boundary()[samplePatchI];

    //tmp<scalarField> intFld = patchInternalField();


    // Calculate the temperature by harmonic averaging
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    const thermalResistanceCoupledBaffleMixedFvPatchScalarField& nbrField =
    refCast
    <
        const thermalResistanceCoupledBaffleMixedFvPatchScalarField
    >
    (
        nbrPatch.lookupPatchField<volScalarField, scalar>
        (
            TnbrName_
        )
    );

    // Swap to obtain full local values of neighbour internal field
    tmp<scalarField> nbrIntFld(new scalarField(nbrField.size(), 0.0));
    tmp<scalarField> nbrKDelta(new scalarField(nbrField.size(), 0.0));

    if (contactRes_ != 0.0)
    {
        nbrIntFld() = nbrField;
        nbrKDelta() = contactRes_;
    }
    else if (resistance_ != 0.0)
    {
        const scalar patchArea = gSum(nbrPatch.magSf());
        nbrIntFld() = nbrField;
        nbrKDelta() = 1/resistance_/patchArea;
    }
    else
    {
        nbrIntFld() = nbrField.patchInternalField();
        nbrKDelta() = nbrField.kappa(nbrField)*nbrPatch.deltaCoeffs();
    }

    mpp.distribute(nbrIntFld());
    mpp.distribute(nbrKDelta());

    tmp<scalarField> myKDelta = kappa(*this)*patch().deltaCoeffs();


    // Both sides agree on
    // - temperature : (myKDelta*fld + nbrKDelta*nbrFld)/(myKDelta+nbrKDelta)
    // - gradient    : (temperature-fld)*delta
    // We've got a degree of freedom in how to implement this in a mixed bc.
    // (what gradient, what fixedValue and mixing coefficient)
    // Two reasonable choices:
    // 1. specify above temperature on one side (preferentially the high side)
    //    and above gradient on the other. So this will switch between pure
    //    fixedvalue and pure fixedgradient
    // 2. specify gradient and temperature such that the equations are the
    //    same on both sides. This leads to the choice of
    //    - refGradient = zero gradient
    //    - refValue = neighbour value
    //    - mixFraction = nbrKDelta / (nbrKDelta + myKDelta())


    this->refValue() = nbrIntFld();

    this->refGrad() = 0.0;

    this->valueFraction() = nbrKDelta()/(nbrKDelta() + myKDelta());

    mixedFvPatchScalarField::updateCoeffs();

    if (debug)
    {
        scalar Q = gSum(kappa(*this)*patch().magSf()*snGrad());

        Info<< patch().boundaryMesh().mesh().name() << ':'
            << patch().name() << ':'
            << this->dimensionedInternalField().name() << " <- "
            << nbrMesh.name() << ':'
            << nbrPatch.name() << ':'
            << this->dimensionedInternalField().name() << " :"
            << " heat transfer rate:" << Q
            << " walltemperature "
            << " min:" << gMin(*this)
            << " max:" << gMax(*this)
            << " avg:" << gAverage(*this)
            << endl;
    }

    // Restore tag
    UPstream::msgType() = oldTag;
}


void thermalResistanceCoupledBaffleMixedFvPatchScalarField::write
(
    Ostream& os
) const
{
    mixedFvPatchScalarField::write(os);
    os.writeKeyword("Tnbr")<< TnbrName_
        << token::END_STATEMENT << nl;
 
    if (contactRes_ != 0.0)
    {
        os.writeKeyword("thicknessLayers")<< thicknessLayers_
            << token::END_STATEMENT << nl;
        os.writeKeyword("kappaLayers")<< kappaLayers_
            << token::END_STATEMENT << nl;
    }
    else if (resistance_ != 0.0)
    {
        os.writeKeyword("resistance")<< resistance_
            << token::END_STATEMENT << nl;
    }



    temperatureCoupledBase::write(os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchScalarField,
    thermalResistanceCoupledBaffleMixedFvPatchScalarField
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace compressible
} // End namespace Foam


// ************************************************************************* //
