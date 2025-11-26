# User Management Report

## Users Created

| User          | UID  | GID  | Home Directory      | Shell         | Purpose               |
|---------------|------|------|---------------------|---------------|-----------------------|
| alice         | 1001 | 1001 | /home/alice         | /bin/bash     | Lead Developer        |
| bob           | 1002 | 1002 | /home/bob           | /bin/bash     | Junior Developer      |
| charlie       | 1003 | 1003 | /home/charlie       | /bin/bash     | QA Tester             |
| temp_user     | 1004 | 1004 | /home/temp_user     | /bin/bash     | Temporary User        |
| service_account | 1005 | -    | -                   | /usr/sbin/nologin | System Account    |

## Groups Created

| Group         | GID  | Members              |
|---------------|------|----------------------|
| developers    | 1006 | alice, bob           |
| testers       | 1007 | charlie              |
| administrators| 1008 | alice                |

## Permissions Matrix

| Directory       | Group Owner  | Permissions | Purpose               |
|-----------------|--------------|-------------|-----------------------|
| /shared/projects| developers   | 770         | Shared dev workspace  |
| /shared/testing | testers      | 775         | Shared testing area   |

## Security Considerations

- Se asignaron contraseñas seguras a todos los usuarios.
- Se configuró expiración para el usuario temporal.
- Se restringió el acceso a directorios compartidos según el grupo.
- Se otorgó acceso `sudo` solo a usuarios autorizados.
