#include <JANA/JFactoryGenerator.h>

#include "SrcqeAnalysisProcessor.h"

extern "C"
{
    void InitPlugin(JApplication *app)
    {
        InitJANAPlugin(app);

        app->Add(new SrcqeAnalysisProcessor(app));
    }
}
