# Endpoints
The API is accessed by making HTTP requests to an endpoint URL, in which GET, POST, PUT, PATCH, and DELETE methods dictate how you interact with the available information.

The default base URL for all the HTTP endpoints is:
> `http://$hostname:$port/libvirt/`

`$hostname` and `$port` being respectively the address and port of the Web API server

# Requests
Requests must be sent over HTTP with any payload formatted in JSON. All requests must include both `X-AUTH-KEY` and `X-AUTH-USERNAME` headers to authenticate. Requests that use `X-AUTH-USER-SERVICE-KEY` can use that instead of the Auth-Key and Auth-Username headers.

## Required parameters
| Name | Format | Description |
| ---- | ------ | ----------- |
| API Key | X-Auth-Key | API key located in config.ini |
| Username | X-Auth-Username | Username associated with a user of group libvirt and kvm |
| User Service Key | X-Auth-User-Service-Key | Special API key good for restricted set of endpoints |

## Example request
Requests generally formatted as follows:
> `GET object/:id_type/:object_id`

#### Auth-Username cURL _(example)_
```bash
curl -X GET "http://127.0.0.1:8081/libvirt/domains/by-uuid/14" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith"
```

#### User-Service cURL _(example)_
```bash
curl -X GET "http://127.0.0.1:8081/libvirt/domains/by-uuid/14" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-User-Service-Key:v1.0-e24fd090c02efcfecb4de8f4ff24..."
```

# Responses
## Format
Each response is a JSON object. The data request is wrapped in the `RESULT` tag. If you have a response, it will always be within the `RESULT` field. We also include a `SUCCESS` flag, an array of potential `ERRORS`, and `MESSAGE` in the response. Some responses can have additional pagination info wrapped in the `RESULT_INFO`

An error object will contain an integer `CODE` field and a `MESSAGE`

Date fields will always be in UTC ISO-8601 format, including microseconds.

#### JSon type response

> { <br/>
> &nbsp;&nbsp;"results": _Array of $result_, <br/>
> &nbsp;&nbsp;"success": _boolean_, <br/>
> &nbsp;&nbsp;"errors": _Array of $error_, <br/>
> &nbsp;&nbsp;"messages": _Array of strings_ <br/>
> }

**$result** is an object _specific to each request_.

**$error** is an object determined with a _CODE_ (integer) and a _MESSAGE_ (string) :

> { <br/>
>   &nbsp;&nbsp;"code": _integer_,<br/>
>   &nbsp;&nbsp;"message": _string_<br/>
> }

For a list of errors, see [Errors](https://github.com/ShinoYasx/HeavyEyelid/wiki/Errors).

### Success Response _(example)_
```json
{
  "results": [
    {
      "uuid": "4dea22b3-1d52-d8f3-2516-782e98ab3fa0",
      "name": "debian-vm",
      "...": "..."
    },
    {
      "uuid": "fa8e1003-1f3f-4a92-a829-66dd523a68f5",
      "name": "manjaro",
      "...": "..."
    }
  ],
  "success": true,
  "errors": [],
  "messages": []
}
```

### Error Response _(example)_
```json
{
  "results": [],
  "success": false,
  "errors": [
    {
      "code": 42,
      "message": "Invalid or missing domain."
    }
  ],
  "messages": []
}
```

# Domains
A Domain is a virtual machine on a host. It can be characterized by a **name** (max 253 characters _string_) or an **UUID** (_string_ of 32 characters separated by `-` : XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX)

## List Domains
List, search, sort, and filter your domains
> `GET domains:flags`
### Optional parameters

| Name _type_ | Description _example_ | Constraints |
| ----------- | --------------------- | ----------- |
| state _number_ **or** _string_ | Domain state _Running_ | See [Status flags](https://github.com/ShinoYasx/HeavyEyelid/wiki/Flags#status-flags) |
| active _boolean_ | Activity specifier | _true_ or _false_ |
| persistent _boolean_ | Persistence specifier | _true_ or _false_ |
| managed_save _boolean_ | Save management policy specifier | _true_ or _false_ |
| has_snapshot _boolean_ | Snaphshot policy specifier | _true_ or _false_ |

N.B. : Flags can be composed with a comma `,` e.g., _?flag=val1,val2_

#### cURL _(example)_
```bash
curl -X GET "http://127.0.0.1:8081/libvirt/domains?state=running&has_snapshot=true" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith"
```

## Create Domain
### with user-provided XML
```bash
curl -X POST "http://127.0.0.1:8081/libvirt/domains" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith" \
     --data '"<domain>...</domain>"'
```
### with auto-generated XML (not implemented)
#### cURL _(example)_
```bash
curl -X POST "http://127.0.0.1:8081/libvirt/domains" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith" \
     --data '{"name":"fedora-vm","os":"Linux"}'
```
You may also send in an array of domains to be created

## Domains Details
> `GET domains/:identifier_type/:identifier`

**:identifier_type** can be _by-name_ or _by-uuid_
#### cURL _(example)_

```bash
curl -X GET "http://127.0.0.1:8081/libvirt/domains/by-uuid/4dea22b3-1d52-d8f3-2516-782e98ab3fa0" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith"
```

### Domains XML description

> `GET domains/:identifier_type/:identifier/xml_desc`

Return the XML description of _:identifier_ as a string.

#### cURL _(example)_

```bash
curl -X GET "http://127.0.0.1:8081/libvirt/domains/by-uuid/4dea22b3-1d52-d8f3-2516-782e98ab3fa0/xml_desc" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith"
```

#### JSon return example

```json
{
  "results": [
    "<domain>...</domain>"
  ],
  "success": true,
  "errors": [],
  "messages": []
}
```

## Manage domains
> `PATCH domains/:identifier_type/:identifier`

### Power management _(JSON grammar)_

The JSon data can be an _obect_ or an _array of objects_ if you need to compose multiple requests relating to this domain.
```json
{
  "power_mgt": "$VALUE"
}
```

**$VALUE** can be a _string_ (see **$ACTION values**) or a _JSON Object_ :

```json
{
  "$VALUE": {
    "request": "$ACTION",
    "type": "$FLAGS"
  }
}
```

For flags, see [Power management flags](https://github.com/ShinoYasx/HeavyEyelid/wiki/Flags#power-management-flags).

If no flags are specified, using no flag.

#### $ACTION values _(according to virsh)_

| Value _(string)_	| Action		|
| ------------------	| ----------	|
| start				| Start a (previously defined) inactive domain |
| shutdown			| Gracefully shutdown a domain |
| destroy			| Destroy (stop) a domain |
| reboot				| Reboot a domain |
| reset				| Reset a domain	|
| suspend			| Suspend (pause) a domain	|
| resume				| Resume (unpause)	|

### cURL _(example: start)_

```bash
curl -X PATCH "http://127.0.0.1:8081/libvirt/domains/by-name/debian-vm" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith" \
     --data '[{"power_mgt":{"request":"start", "type":"ACPI_POWER_BTN"}}]'
```

_N.B. : In this example, the array is useless, it serves only for example purposes._

#### Success response _(example)_
```json
{
  "results": [],
  "success": true,
  "errors": [],
  "messages": [
    {
      "start": "Domain started"
    }
  ]
}
```

#### Error response _(example)_
```json
{
  "results": [],
  "success": false,
  "errors": [
    {
      "code": 201,
      "message": "Domain is already active"
    }
  ],
  "messages": []
}
```

## Delete domains

> `DELETE domains/:identifier_type/:identifier:options`

The request can contain embedded args to specify options. See [Delete flags](https://github.com/ShinoYasx/HeavyEyelid/wiki/Flags#delete-flags). These options may be required, e.g., if your domain has NVRAM, you have to specify if you want to keep the NVRAM file or delete it.

#### cURL _(example)_
```bash
curl -X PATCH "http://127.0.0.1:8081/libvirt/domains/by-name/debian-vm?options=keep_nvram" \
     -H "Content-Type:application/json" \
     -H "X-Auth-Key:1234567893feefc5f0q5000bfo0c38d90bbeb" \
     -H "X-Auth-Username:smith"
```