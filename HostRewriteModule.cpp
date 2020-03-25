#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

// Create the module's global class.
class HostRewriteModule : public CGlobalModule
{
public:

    // Process a GL_PRE_BEGIN_REQUEST notification.
    GLOBAL_NOTIFICATION_STATUS
        OnGlobalPreBeginRequest(
            IN IPreBeginRequestProvider* pProvider
        )
    {
        UNREFERENCED_PARAMETER(pProvider);

        // Create an HRESULT to receive return values from methods.
        HRESULT hr;

        // Specify the "Host" header name.
        char szHostHeaderName[] = "Host";

        // Create buffers to store the returned header value.
        PCSTR pszXForwardedHost;

        // Create buffers to store lengths of the returned header value.
        USHORT cchXForwardedHostLength;

        IHttpContext* pHttpContext = pProvider->GetHttpContext();
        IHttpRequest* pHttpRequest = pHttpContext->GetRequest();

        if (pHttpRequest != NULL)
        {
            // Look for the "X-Forwarded-Host" header.
            pszXForwardedHost = pHttpRequest->GetHeader("X-Forwarded-Host", &cchXForwardedHostLength);

            // The header length will be 0 if the header was not found.
            if (cchXForwardedHostLength != 0)
            {
                // Allocate space to store the "X-Forwarded-Host" header.
                pszXForwardedHost = (PCSTR)pHttpContext->AllocateRequestMemory(cchXForwardedHostLength + 1);

                // Test for an error.
                if (pszXForwardedHost != NULL)
                {
                    // Save the value of the "X-Forwarded-Host" header.
                    pszXForwardedHost = pHttpRequest->GetHeader("X-Forwarded-Host", &cchXForwardedHostLength);

                    // Test for an error.
                    if (pszXForwardedHost != NULL)
                    {
                        // Replace the "Host" header.
                        hr = pHttpRequest->SetHeader(szHostHeaderName, pszXForwardedHost, (USHORT)strlen(pszXForwardedHost), true);

                        // Convert "X-Forwarded-Host" header value from PCSTR to PCWSTR
                        size_t i;
                        mbstowcs_s(&i, nullptr, 0, pszXForwardedHost, _TRUNCATE);

                        wchar_t* httpHost = (wchar_t*)malloc(i * 2);
                        mbstowcs_s(&i, httpHost, i, pszXForwardedHost, _TRUNCATE);

                        // Replace the HTTP_HOST server variable
                        hr = pHttpContext->SetServerVariable("HTTP_HOST", httpHost);
                    }
                }
            }
        }

        // Return processing to the pipeline.
        return GL_NOTIFICATION_CONTINUE;
    }

    VOID Terminate()
    {
        delete this;
    }
};

// Create the module's exported registration function.
HRESULT
__stdcall
RegisterModule(
    DWORD dwServerVersion,
    IHttpModuleRegistrationInfo* pModuleInfo,
    IHttpServer* pGlobalInfo
)
{
    UNREFERENCED_PARAMETER(dwServerVersion);
    UNREFERENCED_PARAMETER(pGlobalInfo);

    // Create an instance of the global module class.
    HostRewriteModule* pGlobalModule = new HostRewriteModule;
    // Test for an error.
    if (NULL == pGlobalModule)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    // Set the global notifications and exit.
    return pModuleInfo->SetGlobalNotifications(
        pGlobalModule, GL_PRE_BEGIN_REQUEST);
}
