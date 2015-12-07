// GLIBManagerWeb.cc

#include "gem/hw/glib/GLIBManagerWeb.h"
#include "gem/hw/glib/GLIBManager.h"
#include "gem/hw/glib/GLIBMonitor.h"

#include "gem/hw/glib/exception/Exception.h"

#include "xcept/tools.h"

gem::hw::glib::GLIBManagerWeb::GLIBManagerWeb(gem::hw::glib::GLIBManager* glibApp) :
  gem::base::GEMWebApplication(glibApp)
{
}

gem::hw::glib::GLIBManagerWeb::~GLIBManagerWeb()
{
  //default destructor
}

void gem::hw::glib::GLIBManagerWeb::webDefault(xgi::Input * in, xgi::Output * out)
  throw (xgi::exception::Exception)
{
  if (p_gemFSMApp)
    DEBUG("current state is" << dynamic_cast<gem::hw::glib::GLIBManager*>(p_gemFSMApp)->getCurrentState());
  *out << cgicc::script().set("type","text/javascript")
    .set("src","/gemdaq/gemhardware/html/scripts/glib/glib.js")
       << cgicc::script() << std::endl;
  *out << "<div class=\"xdaq-tab-wrapper\">" << std::endl;
  *out << "<div class=\"xdaq-tab\" title=\"GLIBManager Control Panel\" >"  << std::endl;
  controlPanel(in,out);
  *out << "</div>" << std::endl;

  *out << "<div class=\"xdaq-tab\" title=\"Monitoring page\"/>"  << std::endl;
  monitorPage(in,out);
  *out << "</div>" << std::endl;

  std::string expURL = "/" + p_gemApp->getApplicationDescriptor()->getURN() + "/expertPage";
  *out << "<div class=\"xdaq-tab\" title=\"Expert page\"/>"  << std::endl;
  expertPage(in,out);
  *out << "</div>" << std::endl;

  std::string cardURL = "/" + p_gemApp->getApplicationDescriptor()->getURN() + "/cardPage";
  *out << "<div class=\"xdaq-tab\" title=\"Card page\"/>"  << std::endl;
  cardPage(in,out);
  *out << "</div>" << std::endl;

  *out << "</div>" << std::endl;
  std::string updateLink = "/" + p_gemApp->m_urn + "/jsonUpdate";
  *out << "<script type=\"text/javascript\">"            << std::endl
       << "    startUpdate( \"" << updateLink << "\" );" << std::endl
       << "</script>" << std::endl;
}

/*To be filled in with the monitor page code*/
void gem::hw::glib::GLIBManagerWeb::monitorPage(xgi::Input * in, xgi::Output * out)
  throw (xgi::exception::Exception)
{
  INFO("monitorPage");
  *out << "<div class=\"xdaq-tab-wrapper\">" << std::endl;
  *out << "<div class=\"xdaq-tab\" title=\"DAQ Link Monitoring\" >"  << std::endl;
  // all monitored GLIBs in one page, or separate tabs?
  //buildDaqLinkMonitoring();
  *out << "</div>" << std::endl;
  *out << "</div>" << std::endl;
}

/*To be filled in with the expert page code*/
void gem::hw::glib::GLIBManagerWeb::expertPage(xgi::Input * in, xgi::Output * out)
  throw (xgi::exception::Exception)
{
  INFO("expertPage");
  //fill this page with the expert views for the GLIBManager
  *out << "expertPage</br>" << std::endl;
}

/*To be filled in with the card page code*/
void gem::hw::glib::GLIBManagerWeb::cardPage(xgi::Input * in, xgi::Output * out)
  throw (xgi::exception::Exception)
{
  INFO("cardPage");
  //fill this page with the card views for the GLIBManager
  *out << "<div class=\"xdaq-tab-wrapper\">" << std::endl;
  for (unsigned int i = 0; i < gem::base::GEMFSMApplication::MAX_AMCS_PER_CRATE; ++i) {
    auto card = dynamic_cast<gem::hw::glib::GLIBManager*>(p_gemFSMApp)->m_glibMonitors[i];
    if (card) {
      *out << "<div class=\"xdaq-tab\" title=\"" << card->getDeviceID() << "\" >"  << std::endl;
      card->buildMonitorPage(out);
      *out << "</div>" << std::endl;
    }
  }
  *out << "</div>" << std::endl;
}

void gem::hw::glib::GLIBManagerWeb::jsonUpdate(xgi::Input * in, xgi::Output * out)
  throw (xgi::exception::Exception)
{
  out->getHTTPResponseHeader().addHeader("Content-Type", "application/json");
  *out << " { \n";
  for (unsigned int i = 0; i < gem::base::GEMFSMApplication::MAX_AMCS_PER_CRATE; ++i) {
    auto card = dynamic_cast<gem::hw::glib::GLIBManager*>(p_gemFSMApp)->m_glibMonitors[i];
    if (card) {
      *out << "\"glib" << std::setw(2) << std::setfill('0') << (i+1) << "\"  : { \n";
      card->jsonUpdateItemSets(out);
      *out << " }, \n";
    }
  }
  *out << " } \n";
}
