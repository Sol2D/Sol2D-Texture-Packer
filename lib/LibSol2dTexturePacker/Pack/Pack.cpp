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

#include <LibSol2dTexturePacker/Pack/Pack.h>
#include <LibSol2dTexturePacker/Exception.h>
#include <QPainter>

class Pack::Texture
{
public:
    bool load(const QString & _filename)
    {
        m_modified_image = QImage();
        m_original_image = QImage();
        return m_original_image.load(_filename);
    }

    bool isLoaded() const
    {
        return !m_original_image.isNull();
    }

    const QImage & image() const
    {
        return m_modified_image.isNull() ? m_original_image : m_modified_image;
    }

    void setColorToAlpha(QRgb _color)
    {
        if(!m_original_image.isNull())
        {
            m_modified_image = m_original_image;
            m_modified_image.setAlphaChannel(m_original_image.createMaskFromColor(_color, Qt::MaskOutColor));
        }
    }

    void removeColorToAlpha()
    {
        m_modified_image = QImage();
    }

private:
    QImage m_original_image;
    QImage m_modified_image;
};

Pack::Pack(const QString & _texture_filename, QObject * _parent) :
    QObject(_parent),
    m_texture_filename(_texture_filename),
    m_texture(new Texture)
{
}

Pack::~Pack()
{
    delete m_texture;
}

void Pack::setColorToAlpha(QRgb _color)
{
    ensureTextureIsLoaded()->setColorToAlpha(_color);
    emit textureChanged();
}

void Pack::removeColorToAlpha()
{
    ensureTextureIsLoaded()->removeColorToAlpha();
    emit textureChanged();
}

Pack::Texture * Pack::ensureTextureIsLoaded() const
{
    if(!m_texture->isLoaded() && !m_texture->load(m_texture_filename))
        throw ImageLoadingException(m_texture_filename);
    return m_texture;
}

void Pack::unpack(const QDir & _output_dir, const QString & _format) const
{
    forEachFrame([&](const Frame & __frame) {
        Sprite sprite = unpackFrame(__frame, _output_dir, _format);
        if(!sprite.image.save(sprite.path))
            throw FileOpenException(sprite.path, FileOpenException::Write);
    });
}

Sprite Pack::unpackFrame(const Frame & _frame, const QDir & _output_dir, const QString & _format) const
{
    QImage texture_sprite = texture().copy(_frame.texture_rect);
    if(_frame.is_rotated)
    {
        QTransform rotation;
        rotation.rotate(-90);
        texture_sprite = texture_sprite.transformed(rotation);
    }
    QImage img(_frame.sprite_rect.width(), _frame.sprite_rect.height(), QImage::Format_RGBA8888);
    img.fill(0);
    QPainter painter(&img);
    painter.drawImage(_frame.sprite_rect.x(), _frame.sprite_rect.y(), texture_sprite);
    const QString filename = makeUnpackFilename(_output_dir, _format, _frame);
    return Sprite { .path = filename, .name = _frame.name, .image = img };
}

QString Pack::makeUnpackFilename(const QDir & _output_dir, const QString & _format, const Frame & _frame) const
{
    const QString base_name = _frame.name.isEmpty() ? "sprite" : QFileInfo(_frame.name).baseName();
    return  QFileInfo(_output_dir.filePath(base_name + "." + _format)).absoluteFilePath();
}

const QImage & Pack::texture() const
{
    return ensureTextureIsLoaded()->image();
}
