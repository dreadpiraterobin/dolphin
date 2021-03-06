// Copyright 2017 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "DolphinQt/NetPlay/GameListDialog.h"

#include <memory>

#include <QDialogButtonBox>
#include <QListWidget>
#include <QVBoxLayout>

#include "DolphinQt/GameList/GameListModel.h"
#include "DolphinQt/Settings.h"
#include "UICommon/GameFile.h"

GameListDialog::GameListDialog(QWidget* parent) : QDialog(parent)
{
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setWindowTitle(tr("Select a game"));

  CreateWidgets();
  ConnectWidgets();
}

void GameListDialog::CreateWidgets()
{
  m_main_layout = new QVBoxLayout;
  m_game_list = new QListWidget;
  m_button_box = new QDialogButtonBox(QDialogButtonBox::Ok);
  m_button_box->setEnabled(false);

  m_main_layout->addWidget(m_game_list);
  m_main_layout->addWidget(m_button_box);

  setLayout(m_main_layout);
}

void GameListDialog::ConnectWidgets()
{
  connect(m_game_list, &QListWidget::itemSelectionChanged,
          [this] { m_button_box->setEnabled(m_game_list->currentRow() != -1); });

  connect(m_game_list, &QListWidget::itemDoubleClicked, this, &GameListDialog::accept);
  connect(m_button_box, &QDialogButtonBox::accepted, this, &GameListDialog::accept);
}

void GameListDialog::PopulateGameList()
{
  auto* game_list_model = Settings::Instance().GetGameListModel();

  m_game_list->clear();

  for (int i = 0; i < game_list_model->rowCount(QModelIndex()); i++)
  {
    std::shared_ptr<const UICommon::GameFile> game = game_list_model->GetGameFile(i);

    auto* item =
        new QListWidgetItem(QString::fromStdString(game_list_model->GetNetPlayName(*game)));
    item->setData(Qt::UserRole, QVariant::fromValue(std::move(game)));
    m_game_list->addItem(item);
  }

  m_game_list->sortItems();
}

const UICommon::GameFile& GameListDialog::GetSelectedGame() const
{
  auto items = m_game_list->selectedItems();
  return *items[0]->data(Qt::UserRole).value<std::shared_ptr<const UICommon::GameFile>>();
}

int GameListDialog::exec()
{
  PopulateGameList();
  return QDialog::exec();
}
