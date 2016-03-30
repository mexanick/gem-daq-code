#include "VFAT_histogram.cxx"
#include "TH1.h"

//!A class that creates histograms for GEB data
class GEB_histogram: public Hardware_histogram
{
  public:
    GEB_histogram(const std::string & filename, TDirectory * dir, const std::string & hwid):Hardware_histogram(filename, dir, hwid){}//call base constructor

    //!Books histograms for GEB data
    /*!
     Books histograms for the following data: Zero Suppresion flags, GLIB input ID, VFAT word count (header), Errors and Warnings (Thirteen Flags, InFIFO underflow flag, Stuck data flag), OH CRC, VFAT word count (trailer)
     */
    void bookHistograms()
    {
      m_dir->cd();
      ZeroSup  = new TH1F("ZeroSup", "Zero Suppression", 0xffffff,  0x0 , 0xffffff);
      InputID  = new TH1F("InputID", "GLIB input ID", 31,  0x0 , 0b11111);      
      Vwh      = new TH1F("Vwh", "VFAT word count", 4095,  0x0 , 0xfff);
      // Assing custom bin labels
      const char *error_flags[5] = {"Event Size Overflow", "L1AFIFO Full", "InFIFO Full", "Evt FIFO Full","InFIFO Underflow"};
      Errors   = new TH1I("Errors", "Critical errors", 5,  0, 5);
      for (int i = 1; i<6; i++) Errors->GetXaxis()->SetBinLabel(i, error_flags[i-1]);
      const char *warning_flags[10] = {"BX AMC-OH Mismatch", "BX AMC-VFAT Mismatch", "OOS AMC OH", "OOS AMC VFAT","No VFAT Marker","Event Size Warn", "L1AFIFO Near Full", "InFIFO Near Full", "EvtFIFO Near Full", "Stuck Data"};
      Warnings = new TH1I("Warnings", "Warnings", 10,  0, 10);
      for (int i = 1; i<11; i++) Warnings->GetXaxis()->SetBinLabel(i, warning_flags[i-1]);
      OHCRC    = new TH1F("OHCRC", "OH CRC", 0xffff,  0x0 , 0xffff);
      Vwt      = new TH1F("Vwt", "VFAT word count", 4095,  0x0 , 0xfff);
    }


    //!Fills histograms for GEB data
    /*!
     Fills the histograms for the following data: Zero Suppresion flags, GLIB input ID, VFAT word count (header), Errors and Warnings (Thirteen Flags, InFIFO underflow flag, Stuck data flag), OH CRC, VFAT word count (trailer)
     */

    void fillHistograms(GEBdata * geb){
      ZeroSup->Fill(geb->ZeroSup());
      InputID->Fill(geb->InputID());
      Vwh->Fill(geb->Vwh());
      //ErrorC->Fill(geb->ErrorC());
      OHCRC->Fill(geb->OHCRC());
      Vwt->Fill(geb->Vwt());
      //InFu->Fill(geb->InFu());
      //Stuckd->Fill(geb->Stuckd());
      uint8_t binFired = 0;
      // Fill Warnings histogram
      for (int bin = 0; bin < 9; bin++){
        binFired = ((geb->ErrorC() >> bin) & 0x1);
        if (binFired) Warnings->Fill(bin);
      }
      binFired = (geb->Stuckd() & 0x1);
      if (binFired) Warnings->Fill(9);
      // Fill Errors histogram
      for (int bin = 9; bin < 13; bin++){
        binFired = ((geb->ErrorC() >> bin) & 0x1);
        if (binFired) Errors->Fill(bin-9);
      }
      binFired = (geb->InFu() & 0x1);
      if (binFired) Warnings->Fill(4);
    }

    //!Adds a VFAT_histogram object to the m_vfatH vector
    void addVFATH(VFAT_histogram vfatH){m_vfatsH.push_back(vfatH);}
    //!Returns the m_vfatsH vector
    std::vector<VFAT_histogram> vfatsH(){return m_vfatsH;}
  private:
    std::vector<VFAT_histogram> m_vfatsH;    ///<A vector of VFAT_histogram 
    TH1F* ZeroSup;
    TH1F* InputID;
    TH1F* Vwh;
    TH1I* Errors;
    TH1I* Warnings;
    TH1F* OHCRC;
    TH1F* Vwt;
};

