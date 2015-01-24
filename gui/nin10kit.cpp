#include "nin10kit.hpp"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>

#include "nin10kitviewer.hpp"

#include "exporter.hpp"
#include "imageutil.hpp"
#include "logger.hpp"

static inline long GetSelectedIndex(wxListCtrl* listControl)
{
    long itemIndex = -1;
    for (;;)
    {
        itemIndex = listControl->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (itemIndex == -1) break;
        return itemIndex;
    }
    return itemIndex;
}

Nin10KitFrame::Nin10KitFrame() : Nin10KitGUI(NULL), imageList(new wxImageList(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT))
{
    imagesList->SetImageList(imageList.get(), wxIMAGE_LIST_NORMAL);
}

Nin10KitFrame::~Nin10KitFrame()
{

}

void Nin10KitFrame::OnLoadImages(wxCommandEvent& event)
{
    EventLog l(__func__);
    std::unique_ptr<wxFileDialog> filedlg(new wxFileDialog(this, _("Open Images"), "", "", wxFileSelectorDefaultWildcardStr,
                                                           wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR | wxFD_MULTIPLE | wxFD_PREVIEW | wxFD_OPEN));
    if (filedlg->ShowModal() == wxID_CANCEL)
    {
        WarnLog("No images given");
        return;
    }

    wxArrayString files;
    filedlg->GetPaths(files);

    for (const auto& wxFile : files)
    {
        std::string file = wxFile.ToStdString();
        InfoLog("Got file: %s", file.c_str());
        if (images.find(file) != images.end())
        {
            WarnLog("File %s already loaded ignoring", file.c_str());
            continue;
        }
        filenames.push_back(file);
        images.emplace(file, file);
        const ImageInfo& info = images.at(file);
        imageList->Add(info.GetThumbnail());
        long index = imagesList->GetItemCount();
        imagesList->InsertItem(index, info.GetName(), index);
    }
}

void Nin10KitFrame::OnImageSelected(wxListEvent& event)
{
    EventLog l(__func__);
    //Ignoring event.GetSelection as it is always 0.
    long index = GetSelectedIndex(imagesList);
    if (index == -1)
    {
        WarnLog("No item selected ignoring");
        return;
    }

    const std::string& filename = filenames[index];
    const ImageInfo& info = images.at(filename);
    InfoLog("Selected %d %s %s", index, filename.c_str(), info.GetName().c_str());

    imageFilename->SetValue(filename);
    imageName->SetValue(info.GetName());
    imageWidth->SetValue(info.GetWidth());
    imageHeight->SetValue(info.GetHeight());
    imageAnimated->SetValue(info.IsAnimated());
}

void Nin10KitFrame::OnDeleteAllImages(wxCommandEvent& event)
{
    EventLog l(__func__);
    InfoLog("Delete all items");
    imagesList->DeleteAllItems();
    imageList->RemoveAll();
    filenames.clear();
    images.clear();
}

void Nin10KitFrame::OnUpdateCurrentImage(wxCommandEvent& event)
{
    EventLog l(__func__);
    long item = GetSelectedIndex(imagesList);
    if (item == -1)
    {
        WarnLog("No item selected ignoring");
        return;
    }

    const std::string& filename = filenames[item];
    ImageInfo& info = images.at(filename);
    InfoLog("Updating %ld %s %s", item, filename.c_str(), info.GetName().c_str());

    info.SetName(imageName->GetValue().ToStdString());
    info.SetWidth(imageWidth->GetValue());
    info.SetHeight(imageHeight->GetValue());
    InfoLog("Updated %ld %s %s", item, filename.c_str(), info.GetName().c_str());
    imagesList->SetItemText(item, info.GetName());
    imagesList->Refresh();
}

void Nin10KitFrame::OnDeleteCurrentImage(wxCommandEvent& event)
{
    EventLog l(__func__);
    long item = GetSelectedIndex(imagesList);
    if (item == -1)
    {
        WarnLog("No item selected ignoring");
        return;
    }

    imagesList->DeleteItem(item);
    imageList->Remove(item);
    InfoLog("Updating %ld %s %s", item, filenames[item].c_str(), images.at(filenames[item]).GetName().c_str());
    images.erase(filenames[item]);
    filenames.erase(filenames.begin() + item);
}

void Nin10KitFrame::OnExport(wxCommandEvent& event)
{
    EventLog l(__func__);
    if (filenames.empty())
    {
        WarnLog("No images loaded not exporting");
        return;
    }
    std::string filename = wxSaveFileSelector(_("Export Images"), "", "", this).ToStdString();
    if (filename.empty())
    {
        WarnLog("Filename empty not saving");
        return;
    }

    InfoLog("Saving exported file to %s", filename.c_str());
    DoExport(mode->GetSelection(), filename, filenames, images);
}

void Nin10KitFrame::OnView(wxCommandEvent& event)
{
    EventLog l(__func__);
    if (images.empty())
    {
        WarnLog("No images given.");
        return;
    }

    if (!viewer)
        viewer = new Nin10KitViewerFrame();

    viewer->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Nin10KitFrame::OnCloseView), NULL, this);
    viewer->Set(mode->GetSelection(), images);
    viewer->Show(true);
}

void Nin10KitFrame::OnCloseView(wxCloseEvent& event)
{
    EventLog l(__func__);
    if (viewer)
        viewer = NULL;
    event.Skip();
}

void Nin10KitFrame::OnEditor(wxCommandEvent& event)
{
    EventLog l(__func__);
    wxMessageBox(_("Please pay me 20 dolla for the full version"), _("GET OUT MY STORE"));
}
