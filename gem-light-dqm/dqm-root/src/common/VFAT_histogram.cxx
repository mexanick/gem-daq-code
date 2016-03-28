#include "Hardware_histogram.h"
#include "TH1.h"
#include <Event.h>
#define NCHANNELS 128

class VFAT_histogram: public Hardware_histogram
{
  public:
    VFAT_histogram(const std::string & filename, TDirectory * dir, const std::string & hwid):Hardware_histogram(filename, dir, hwid){}//call base constructor
    VFAT_histogram(VFAT_histogram * vH):Hardware_histogram("dummy", vH->m_dir, vH->m_HWID){}//call base constructor
    ~VFAT_histogram(){}
    void bookHistograms(){
      m_dir->cd();
      b1010    = new TH1F("b1010", "Control Bits", 15,  0x0 , 0xf);
      BC       = new TH1F("BC", "Bunch Crossing Number", 4095,  0x0 , 0xfff);
      b1100    = new TH1F("b1100", "Control Bits", 15,  0x0 , 0xf);
      EC       = new TH1F("EC", "Event Counter", 255,  0x0 , 0xff);
      Flag     = new TH1F("Flag", "Control Flags", 15,  0x0 , 0xf);
      b1110    = new TH1F("b1110", "Control Bits", 15,  0x0 , 0xf);
      ChipID   = new TH1F("ChipID", "Chip ID", 4095,  0x0 , 0xfff);
      FiredChannels   = new TH1F("FiredChannels", "FiredChannels", 128,  0, 128);
      crc_difference = new TH1F("crc_difference", "difference between crc and crc_calc", 0xffff,  -32768 , 32768);
      TDirectory * scandir = gDirectory->mkdir("Threshold_Scans");
      scandir->cd();
      for (int i = 0; i < 128; i++){
        thresholdScan[i] = new TH1F(("thresholdScan"+to_string(static_cast<long long int>(i))).c_str(),("thresholdScan"+to_string(static_cast<long long int>(i))).c_str(),256,0,256);
      }// end loop on channels
      gDirectory->cd("..");
    }
    void fillHistograms(VFATdata * vfat){
      setVFATBlockWords(vfat); 
      crc_difference->Fill(vfat->crc()-checkCRC(vfatBlockWords));  
      b1010->Fill(vfat->b1010());
      b1100->Fill(vfat->b1100());
      b1110->Fill(vfat->b1110());
      BC->Fill(vfat->BC());
      EC->Fill(vfat->EC());
      Flag->Fill(vfat->Flag());
      ChipID->Fill(vfat->ChipID());
      uint16_t chan0xf = 0;
      for (int chan = 0; chan < 128; ++chan) {
        if (chan < 64){
          chan0xf = ((vfat->lsData() >> chan) & 0x1);
          if(chan0xf) FiredChannels->Fill(chan);
        } else {
          chan0xf = ((vfat->msData() >> (chan-64)) & 0x1);
          if(chan0xf) FiredChannels->Fill(chan);
        }
      }
    }
    void fillScanHistograms(VFATdata * vfat, int runtype, int deltaV){
      for (int i = 0; i < 128; i++){
        uint16_t chan0xf = 0;
        if (i < 64){
          chan0xf = ((vfat->lsData() >> i) & 0x1);
          if(chan0xf) thresholdScan[i]->Fill(deltaV);
        } else {
          chan0xf = ((vfat->msData() >> (i-64)) & 0x1);
          if(chan0xf) thresholdScan[i]->Fill(deltaV);
        }
      }// end loop on channels
    }
  private:
    TH1F* b1010;
    TH1F* BC;
    TH1F* b1100;
    TH1F* EC;
    TH1F* Flag;
    TH1F* b1110;
    TH1F* ChipID;
    TH1F* FiredChannels;
    TH1F* crc_difference;
    TH1F* thresholdScan[NCHANNELS]; 

    uint16_t vfatBlockWords[12];
    void setVFATBlockWords(VFATdata * vfat_)
    {
      vfatBlockWords[11] = vfat_->BC();
      vfatBlockWords[10] = vfat_->EC();
      vfatBlockWords[9]  = vfat_->ChipID();
      vfatBlockWords[8]  = (0xffff000000000000 & vfat_->msData()) >> 48;
      vfatBlockWords[7]  = (0x0000ffff00000000 & vfat_->msData()) >> 32;
      vfatBlockWords[6]  = (0x00000000ffff0000 & vfat_->msData()) >> 16;
      vfatBlockWords[5]  = (0x000000000000ffff & vfat_->msData());
      vfatBlockWords[4]  = (0xffff000000000000 & vfat_->lsData()) >> 48;
      vfatBlockWords[3]  = (0x0000ffff00000000 & vfat_->lsData()) >> 32;
      vfatBlockWords[2]  = (0x00000000ffff0000 & vfat_->lsData()) >> 16;
      vfatBlockWords[1]  = (0x000000000000ffff & vfat_->lsData());
     }

    
    uint16_t checkCRC(uint16_t dataVFAT[11])
       {
         uint16_t crc_fin = 0xffff;
         for (int i = 11; i >= 1; i--)
         {
           crc_fin = this->crc_cal(crc_fin, dataVFAT[i]);
         }
         return(crc_fin);
       }

    uint16_t crc_cal(uint16_t crc_in, uint16_t dato)
       {
         uint16_t v = 0x0001;
         uint16_t mask = 0x0001;    
         bool d=0;
         uint16_t crc_temp = crc_in;
         unsigned char datalen = 16;
          
         for (int i=0; i<datalen; i++){
           if (dato & v) d = 1;
           else d = 0;
           if ((crc_temp & mask)^d) crc_temp = crc_temp>>1 ^ 0x8408;
           else crc_temp = crc_temp>>1;
           v<<=1;
         }
         return(crc_temp);
       }
    
};
