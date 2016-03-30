#include "GEB_histogram.cxx"
#include "TH1.h"

//!A class creating histograms for AMC data
class AMC_histogram: public Hardware_histogram
{
  public:
    AMC_histogram(const std::string & filename, TDirectory * dir, const std::string & hwid):Hardware_histogram(filename, dir, hwid){}//call base constructor

    //!Books histograms for AMC data
    /*!
    This books histograms for the following data: AMC number, L1A number, Bunch Crossing ID, Data Length, Format Version, Run Type, Param 1, 2 and 3, Orbit Number, Board ID, GEM DAV list, Buffer Status, GEM DAV count, TTS state, Chamber Timeout, OOS GLIB sync, CRC, L1AT, and DlengthT
    */
    void bookHistograms()
    {
      m_dir->cd();
      AMCnum     = new TH1F("AMCnum", "AMC number", 12,  0, 12);
      L1A        = new TH1F("L1A", "L1A ID", 0xffffff,  0x0, 0xffffff);      
      BX         = new TH1F("BX", "BX ID", 4095,  0x0, 0xfff);
      Dlength    = new TH1F("Dlength", "Data Length", 0xfffff,  0x0, 0xfffff);
      FV         = new TH1F("FV", "Format Version", 15,  0x0, 0xf);
      Rtype      = new TH1F("Rtype", "Run Type", 15,  0x0, 0xf);
      Param1     = new TH1F("Param1", "Run Param 1", 255,  0, 255);
      Param2     = new TH1F("Param2", "Run Param 2", 255,  0, 255);
      Param3     = new TH1F("Param3", "Run Param 3", 255,  0, 255);
      Onum       = new TH1F("Onum", "Orbit Number", 0xffff,  0, 0xffff);
      BID        = new TH1F("BID", "Board ID", 0xffff,  0, 0xffff);
      GEMDAV     = new TH1F("GEMDAV", "GEM DAV list", 24,  0, 24);
      Bstatus    = new TH1F("Bstatus", "Buffer Status", 24,  0, 24);
      GDcount    = new TH1F("GDcount", "GEM DAV count", 32,  0, 32);
      Tstate     = new TH1F("Tstate", "TTS state", 15,  0, 15);
      ChamT      = new TH1F("ChamT", "Chamber Timeout", 24, 0, 24);
      OOSG       = new TH1F("OOSG", "OOS GLIB", 1, 0, 1);
      CRC        = new TH1D("CRC", "CRC", 4294967295, 0, 4294967295);// histogram overflow! Can't handle 32-bit number...
      L1AT       = new TH1F("L1AT", "L1AT", 0xffffff,  0x0, 0xffffff);
      DlengthT   = new TH1F("DlengthT", "DlengthT", 0xffffff,  0x0, 0xffffff);
    }

     //!Fills the histograms for AMC data
    /*!
    This fills the histograms for the following data: AMC number, L1A number, Bunch Crossing ID, Data Length, Format Version, Run Type, Param 1, 2 and 3, Orbit Number, Board ID, GEM DAV list, Buffer Status, GEM DAV count, TTS state, Chamber Timeout, OOS GLIB sync, CRC, L1AT, and DlengthT
    */
    void fillHistograms(AMCdata *amc){
      AMCnum->Fill(amc->AMCnum());
      L1A->Fill(amc->L1A());
      BX->Fill(amc->BX());
      Dlength->Fill(amc->Dlength());
      FV->Fill(amc->FV());
      Rtype->Fill(amc->Rtype());
      Param1->Fill(amc->Param1());
      Param2->Fill(amc->Param2());
      Param3->Fill(amc->Param3());
      Onum->Fill(amc->Onum());
      BID->Fill(amc->BID());
      GDcount->Fill(amc->GDcount());
      Tstate->Fill(amc->Tstate());
      OOSG->Fill(amc->OOSG());
      CRC->Fill(amc->CRC());
      L1AT->Fill(amc->L1AT());
      DlengthT->Fill(amc->DlengthT());
      uint8_t binFired = 0;
      for (int bin = 0; bin < 24; bin++){
        binFired = ((amc->GEMDAV() >> bin) & 0x1);
        if (binFired) GEMDAV->Fill(bin);
        binFired = ((amc->Bstatus() >> bin) & 0x1);
        if (binFired) Bstatus->Fill(bin);
        binFired = ((amc->ChamT() >> bin) & 0x1);
        if (binFired) ChamT->Fill(bin);
      }
    }
    //!Adds a GEB_histogram object to m_gebsH vector
    void addGEBH(GEB_histogram gebH){m_gebsH.push_back(gebH);}
    //!Returns the m_gebsH vector
    std::vector<GEB_histogram> gebsH(){return m_gebsH;}
  private:
    std::vector<GEB_histogram> m_gebsH;   ///<A vector of GEB_histogram
    TH1F* AMCnum;
    TH1F* L1A;
    TH1F* BX;
    TH1F* Dlength;
    TH1F* FV;
    TH1F* Rtype;
    TH1F* Param1;
    TH1F* Param2;
    TH1F* Param3;
    TH1F* Onum;
    TH1F* BID;
    TH1F* GEMDAV;
    TH1F* Bstatus;
    TH1F* GDcount;
    TH1F* Tstate;
    TH1F* ChamT;
    TH1F* OOSG;
    TH1D* CRC;
    TH1F* L1AT;
    TH1F* DlengthT;
};
