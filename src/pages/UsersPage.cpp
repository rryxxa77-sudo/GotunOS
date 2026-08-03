#include "UsersPage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>
#include <QRegularExpression>

static QLabel *fieldLbl(const QString &t) {
    auto *l = new QLabel(t);
    l->setObjectName("fieldLabel");
    l->setMinimumWidth(120);
    return l;
}

UsersPage::UsersPage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(14);

    auto *tt = new QLabel(I18n::tr("users_title")); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("users_subtitle"));
    ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(4);

    // ── User rows ─────────────────────────────────────────────────────────────
    auto *usersLabel = new QLabel(I18n::tr("users_section"));
    usersLabel->setObjectName("sectionHeader");
    lay->addWidget(usersLabel);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setFixedHeight(180);
    m_rowsWidget = new QWidget();
    m_rowsLayout = new QVBoxLayout(m_rowsWidget);
    m_rowsLayout->setContentsMargins(0,0,0,0);
    m_rowsLayout->setSpacing(8);
    scroll->setWidget(m_rowsWidget);
    lay->addWidget(scroll);

    auto *btnAdd = new QPushButton(I18n::tr("add_user"));
    btnAdd->setObjectName("btnSmall");
    btnAdd->setFixedWidth(140);
    connect(btnAdd, &QPushButton::clicked, this, &UsersPage::addUserRow);
    lay->addWidget(btnAdd);

    // ── Root password ─────────────────────────────────────────────────────────
    auto *sep = new QFrame(); sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:#e5e5ea; margin-top:6px; margin-bottom:6px;");
    lay->addWidget(sep);

    auto *rl = new QLabel(I18n::tr("root_section"));
    rl->setObjectName("sectionHeader");
    lay->addWidget(rl);

    m_samePassChk = new QCheckBox(I18n::tr("same_root_pass"));
    m_samePassChk->setChecked(true);
    lay->addWidget(m_samePassChk);

    auto *rpRow = new QHBoxLayout(); rpRow->setSpacing(10);
    rpRow->addWidget(fieldLbl(I18n::tr("root_pass")));
    m_rootPass = new QLineEdit(); m_rootPass->setEchoMode(QLineEdit::Password);
    m_rootPass->setPlaceholderText("Root password");
    rpRow->addWidget(m_rootPass);
    m_rootConfirm = new QLineEdit(); m_rootConfirm->setEchoMode(QLineEdit::Password);
    m_rootConfirm->setPlaceholderText("Confirm");
    rpRow->addWidget(m_rootConfirm);
    lay->addLayout(rpRow);

    m_noPassSudo = new QCheckBox(I18n::tr("nopass_sudo"));
    m_noPassSudo->setChecked(true);
    lay->addWidget(m_noPassSudo);

    connect(m_samePassChk, &QCheckBox::toggled, this, [this](bool checked) {
        m_rootPass->setEnabled(!checked);
        m_rootConfirm->setEnabled(!checked);
        if (checked) syncSamePassword();
    });

    lay->addStretch();

    addUserRow();
}

void UsersPage::addUserRow() {
    int idx = m_rows.size();
    UserRow row;

    auto *frame = new QFrame();
    frame->setObjectName("card");
    auto *fl = new QHBoxLayout(frame);
    fl->setContentsMargins(14, 10, 14, 10);
    fl->setSpacing(10);

    auto *label = new QLabel(QString(I18n::language() == "tr" ? "Kullanıcı %1:" : "User %1:").arg(idx + 1));
    label->setObjectName("fieldLabel");
    label->setMinimumWidth(80);
    fl->addWidget(label);
    row.label = label;

    row.username = new QLineEdit(); row.username->setPlaceholderText(I18n::language() == "tr" ? "Kullanıcı adı (ör. alex)" : "Username (e.g. alex)");
    row.password = new QLineEdit(); row.password->setPlaceholderText(I18n::language() == "tr" ? "Şifre" : "Password");
    row.password->setEchoMode(QLineEdit::Password);
    row.confirm  = new QLineEdit(); row.confirm->setPlaceholderText(I18n::language() == "tr" ? "Şifreyi onayla" : "Confirm password");
    row.confirm->setEchoMode(QLineEdit::Password);
    fl->addWidget(row.username);
    fl->addWidget(row.password);
    fl->addWidget(row.confirm);

    if (idx == 0) {
        connect(row.password, &QLineEdit::textChanged, this, [this](const QString &t) {
            Q_UNUSED(t);
            if (m_samePassChk->isChecked()) syncSamePassword();
        });
        connect(row.confirm, &QLineEdit::textChanged, this, [this](const QString &t) {
            Q_UNUSED(t);
            if (m_samePassChk->isChecked()) syncSamePassword();
        });
    } else {
        row.remove = new QPushButton("X");
        row.remove->setObjectName("btnDanger");
        row.remove->setFixedSize(32, 32);
        auto *usernameEdit = row.username;
        connect(row.remove, &QPushButton::clicked, this, [this, frame, usernameEdit]() {
            // Look the row up by widget: indices shift once a row is removed.
            for (int i = 0; i < m_rows.size(); ++i)
                if (m_rows[i].username == usernameEdit) { m_rows.removeAt(i); break; }
            frame->deleteLater();
            renumberRows();
        });
        fl->addWidget(row.remove);
    }

    m_rows.append(row);
    m_rowsLayout->addWidget(frame);

    if (m_samePassChk->isChecked()) syncSamePassword();
}

void UsersPage::syncSamePassword() {
    if (m_rows.isEmpty()) return;
    const QString pass = m_rows.first().password->text();
    m_rootPass->setText(pass);
    m_rootConfirm->setText(pass);
    m_rootPass->setEnabled(false);
    m_rootConfirm->setEnabled(false);
}

void UsersPage::renumberRows() {
    for (int i = 0; i < m_rows.size(); ++i)
        if (m_rows[i].label)
            m_rows[i].label->setText(QString(I18n::language() == "tr" ? "Kullanıcı %1:" : "User %1:").arg(i + 1));
}

void UsersPage::commitToConfig() {
    m_cfg->users.clear();
    for (const auto &r : m_rows) {
        const QString u = r.username->text().trimmed();
        const QString p = r.password->text();
        if (!u.isEmpty()) m_cfg->users.append({u, p});
    }

    if (m_samePassChk->isChecked() && !m_rows.isEmpty()) {
        m_cfg->rootPassword = m_rows.first().password->text();
    } else {
        m_cfg->rootPassword = m_rootPass->text();
    }
    m_cfg->passwordlessSudo = m_noPassSudo->isChecked();
}

bool UsersPage::isValid() {
    const bool tr = (I18n::language() == "tr");
    // Same rule useradd enforces on Arch.
    static const QRegularExpression nameRe("^[a-z_][a-z0-9_-]*$");

    int named = 0;
    for (int i = 0; i < m_rows.size(); ++i) {
        const auto &r = m_rows[i];
        const QString name = r.username->text().trimmed();
        if (name.isEmpty() && m_rows.size() > 1) continue; // spare blank row

        if (!nameRe.match(name).hasMatch() || name.size() > 32) {
            QMessageBox::warning(this, I18n::tr("users_title"), tr
                ? "Kullanıcı adı küçük harf veya alt çizgi ile başlamalı; sadece küçük harf, rakam, - ve _ içerebilir."
                : "A username has to start with a lowercase letter or underscore and can only contain "
                  "lowercase letters, digits, '-' and '_'.");
            r.username->setFocus();
            return false;
        }
        ++named;

        for (int j = 0; j < i; ++j) {
            if (m_rows[j].username->text().trimmed() == name) {
                QMessageBox::warning(this, I18n::tr("users_title"), tr
                    ? "Aynı kullanıcı adı iki kez kullanılamıyor."
                    : "That username is used twice.");
                r.username->setFocus();
                return false;
            }
        }

        if (r.password->text().isEmpty()) {
            QMessageBox::warning(this, I18n::tr("users_title"), tr
                ? "Boş şifre kullanılamaz." : "Empty passwords are not allowed.");
            r.password->setFocus();
            return false;
        }

        if (r.password->text() != r.confirm->text()) {
            QMessageBox::warning(this, I18n::tr("users_title"), tr
                ? "Kullanıcı şifreleri eşleşmiyor!" : "User passwords do not match!");
            r.password->setFocus();
            return false;
        }
    }

    if (named == 0) {
        QMessageBox::warning(this, I18n::tr("users_title"), tr
            ? "En az bir kullanıcı hesabı gerekli." : "You need at least one user account.");
        return false;
    }

    if (!m_samePassChk->isChecked()) {
        if (m_rootPass->text().isEmpty() || m_rootPass->text() != m_rootConfirm->text()) {
            QMessageBox::warning(this, I18n::tr("users_title"), tr
                ? "Root şifresi boş olamaz ve iki alan eşleşmeli!"
                : "The root password cannot be empty and both fields have to match!");
            m_rootPass->setFocus();
            return false;
        }
    }

    return true;
}
