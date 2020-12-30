#include <fstream>

#include "spdlog/spdlog.h"
#include "App.h"

#define CHUNK_SZ 256 * 1024
#define PORT 6123

typedef uWS::HttpResponse<false> HttpResponse;

inline bool hasExt ( const std::string& file, const std::string& ext )
{
    if ( ext.size() > file.size() ) {
        return false;
    }

    return std::equal ( ext.rbegin(), ext.rend(), file.rbegin() );
}

bool validateContentType ( const std::string& path, HttpResponse* res )
{
    if ( hasExt ( path, ".html" ) ) {
        res->writeHeader ( "Content-Type", "text/html" );
    } else if ( hasExt ( path, ".css" ) ) {
        res->writeHeader ( "Content-Type", "text/css" );
    } else if ( hasExt ( path, ".js" ) ) {
        res->writeHeader ( "Content-Type", "text/javascript" );
    } else if ( hasExt ( path, ".ico" ) ) {
        res->writeHeader ( "Content-Type", "image/x-icon" );
    } else {
      /*  res->writeStatus ( "403 Forbidden" )->end ( "Forbidden\n" );
        return false; */
    }

    res->writeStatus ( uWS::HTTP_200_OK );
    return true;
}

void fsHandler ( HttpResponse* res, uWS::HttpRequest* req )
{
    auto url = req->getUrl();
    std::string path = url == "/" ? "/index.html" : std::string ( url );

    spdlog::debug("Access path: {}", path);
    std::ifstream in ( path.substr ( 1 ) );
    if ( !in ) {
        res->writeStatus ( "404 Not Found" )->end ( "Not Found\n" );
        return;
    }

    if ( !validateContentType ( path, res ) ) {
        return;
    }

    std::unique_ptr<char[]> buffer ( new char[CHUNK_SZ] );
    while ( !in.eof() ) {
        in.read ( buffer.get(), CHUNK_SZ );
        auto read = in.gcount();
        std::string_view sv ( buffer.get(), read );
        res->write ( sv );
    }

    res->end();
}

int main()
{
    spdlog::set_level(spdlog::level::debug);
    
    uWS::App()
    .get ( "/*", &fsHandler )
    .listen ( PORT, [] ( auto *listen_socket ) {
        if ( listen_socket ) {
            spdlog::info("Listening on port {}", PORT);
        }
    } )
    .run();

    spdlog::critical("Failed to listen on port {}", PORT);
}

