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

Class
    Foam::
    compressible::
    thermalResistanceCoupledBaffleMixedFvPatchScalarField

Description
    Mixed boundary condition for temperature, to be used for heat-transfer
    on back-to-back baffles.Optional thin thermal layer
    resistances can be specified through thicknessLayers and kappaLayers
    entries.

    The thermal conductivity, \c kappa, can either be retrieved from the
    mesh database using the \c lookup option, or from a \c solidThermo
    or \c fluidThermo thermophysical package.


    Specifies gradient and temperature such that the equations are the same
    on both sides:
    - refGradient = zero gradient
    - refValue = neighbour value
    - mixFraction = nbrKDelta / (nbrKDelta + myKDelta())

    where KDelta is heat-transfer coefficient K * deltaCoeffs

    \heading Patch usage

    \table
        Property     | Description             | Required    | Default value
        kappa        | thermal conductivity option | yes     |
        kappaName    | name of thermal conductivity field | no  |
        Tnbr         | name of the field    | no | T
        thicknessLayers | list of thicknesses per layer [m] | no |
        kappaLayers  | list of thermal conductivites per layer [W/m/K] | no |
    \endtable

    Example of the boundary condition specification:
    \verbatim
    myPatch
    {
        type            compressible::thermalResistanceCoupledBaffleMixed;
        Tnbr            T;
        kappa           lookup;
        KappaName       kappa;
        thicknessLayers (0.1 0.2 0.3 0.4);
        kappaLayers     (1 2 3 4)
        value           uniform 300;
    }
    \endverbatim

    Needs to be on underlying mapped(Wall)FvPatch.

    Note: kappa : heat conduction at patch. Gets supplied how to lookup
        calculate kappa:

    - 'lookup' : lookup volScalarField (or volSymmTensorField) with name
    - 'fluidThermo' : use fluidThermo and compressible::RASmodel to calculate
        kappa
    - 'solidThermo' : use solidThermo kappa()
    - 'directionalSolidThermo' directionalKappa()

SourceFiles
    thermalResistanceCoupledBaffleMixedFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#ifndef thermalResistanceCoupledBaffleMixedFvPatchScalarField_H
#define thermalResistanceCoupledBaffleMixedFvPatchScalarField_H

#include "mixedFvPatchFields.H"
#include "temperatureCoupledBase.H"
#include "scalarField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace compressible
{

/*---------------------------------------------------------------------------*\
   Class thermalResistanceCoupledBaffleMixedFvPatchScalarField Declaration
\*---------------------------------------------------------------------------*/

class thermalResistanceCoupledBaffleMixedFvPatchScalarField
:
    public mixedFvPatchScalarField,
    public temperatureCoupledBase
{
    // Private data

        //- Name of field on the neighbour region
        const word TnbrName_;

        //- Thickness of layers
        scalarList thicknessLayers_;

        //- Conductivity of layers
        scalarList kappaLayers_;

        //- Total contact resistance
        scalar contactRes_;

        //- Total thermal resistance
        scalar resistance_;


public:

    //- Runtime type information
    TypeName("compressible::thermalResistanceCoupledBaffleMixed");


    // Constructors

        //- Construct from patch and internal field
        thermalResistanceCoupledBaffleMixedFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        thermalResistanceCoupledBaffleMixedFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given
        //  thermalResistanceCoupledBaffleMixedFvPatchScalarField onto a
        //  new patch
        thermalResistanceCoupledBaffleMixedFvPatchScalarField
        (
            const thermalResistanceCoupledBaffleMixedFvPatchScalarField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchScalarField> clone() const
        {
            return tmp<fvPatchScalarField>
            (
                new thermalResistanceCoupledBaffleMixedFvPatchScalarField
                (
                    *this
                )
            );
        }

        //- Construct as copy setting internal field reference
        thermalResistanceCoupledBaffleMixedFvPatchScalarField
        (
            const thermalResistanceCoupledBaffleMixedFvPatchScalarField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchScalarField> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchScalarField>
            (
                new thermalResistanceCoupledBaffleMixedFvPatchScalarField
                (
                    *this,
                    iF
                )
            );
        }


    // Member functions

        //- Update the coefficients associated with the patch field
        virtual void updateCoeffs();

        //- Write
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace compressible
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
