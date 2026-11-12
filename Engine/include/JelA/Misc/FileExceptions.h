#ifndef FILEEXCEPTIONS_H
#define FILEEXCEPTIONS_H

#include <format>
#include <stdexcept>

namespace jela
{
    class FileException : public std::runtime_error
    {
    public :
        explicit FileException(const std::string& info) :
            runtime_error(std::format("FileException: {}", info))
        {
        }
    };

    class FileTypeNotSupportedException final : public FileException
    {
    public:
        explicit FileTypeNotSupportedException(const std::string& info,
                                               const std::initializer_list<std::string>& supportedExtensions)
            : FileException("FileTypeNotSupportedException: ")
        {
            m_Output = std::format("{}{} Expected formats are", std::string{ FileException::what() }, info);

            for (int i = 0; i < supportedExtensions.size(); ++i)
            {
                if (i == supportedExtensions.size() - 1) m_Output += std::format(" {}", supportedExtensions.begin()[i]);
                else m_Output += std::format(" {},", supportedExtensions.begin()[i]);
            }

            m_Output += ".\n";
        }

        const char* what() const noexcept override
        {
            return m_Output.c_str();
        }

    private:
        std::string m_Output{};
    };

    class FileNotFoundException final : public FileException
    {
    public:
        explicit FileNotFoundException(const std::string& info)
            : FileException(std::format("FileNotFoundException: {}", info))
        {
        }
    };

    class FileLoadException final : public FileException
    {
    public:
        explicit FileLoadException(const std::string& info)
            : FileException(std::format("FileLoadException: {}", info))
        {
        }
    };
}

#endif //FILEEXCEPTIONS_H