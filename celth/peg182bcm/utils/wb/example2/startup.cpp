


#include "peg.hpp"
#include "Exam2.hpp"

void PegAppInitialize(PegPresentationManager *pPresent)
{
    ExampleTwo *pWin = new ExampleTwo(10, 10);
    pPresent->Center(pWin);
    pPresent->Add(pWin);
}