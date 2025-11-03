/**********************************************************************************************************
 * Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   *
 *                                                                                                        *
 * This file is part of the Sol2D Texture Packer.                                                         *
 *                                                                                                        *
 * Sol2D Texture Packer is free software: you can redistribute it and/or modify it under  the terms of    *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 of the   *
 * License, or (at your option) any later version.                                                        *
 *                                                                                                        *
 * Sol2D Texture Packer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             *
 * See the GNU General Public License for more details.                                                   *
 *                                                                                                        *
 * You should have received a copy of the GNU General Public License along with MailUnit.                 *
 * If not, see <http://www.gnu.org/licenses/>.                                                            *
 *                                                                                                        *
 **********************************************************************************************************/

#include <Sol2dTexturePackerGui/ColorPickerInput.h>
#include <QColorDialog>

ColorPickerInput::ColorPickerInput(QWidget * _parent) :
    QWidget(_parent),
    m_is_alpha_enabled(false)
{
    setupUi(this);
    connect(m_btm_pick, &QPushButton::clicked, this, &ColorPickerInput::pickColor);
    setColor(QColor());
}

void ColorPickerInput::setColor(const QColor & _color) {
    m_current_color = _color;
    if(m_is_alpha_enabled)
    {
        m_edit_color->setText(QString("#%1%2%3%4")
            .arg(m_current_color.red(), 2, 16, '0')
            .arg(m_current_color.green(), 2, 16, '0')
            .arg(m_current_color.blue(), 2, 16, '0')
            .arg(m_current_color.alpha(), 2, 16, '0'));
    }
    else
    {
        m_edit_color->setText(QString("#%1%2%3")
        .arg(m_current_color.red(), 2, 16, '0')
        .arg(m_current_color.green(), 2, 16, '0')
        .arg(m_current_color.blue(), 2, 16, '0'));
    }
}

void ColorPickerInput::enableAlphaChannel(bool _enable)
{
    if(_enable != m_is_alpha_enabled)
    {
        m_is_alpha_enabled = _enable;
        setColor(m_current_color);
    }
}

void ColorPickerInput::clear()
{
    m_current_color = QColor();
    m_edit_color->clear();
}

void ColorPickerInput::pickColor()
{
    QColorDialog dlg(m_current_color, this);
    dlg.setOption(QColorDialog::ShowAlphaChannel, m_is_alpha_enabled);
    if(dlg.exec() == QDialog::Accepted)
    {
        setColor(dlg.currentColor());
        emit colorChanged();
    }
}
