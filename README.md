Description
-----------
The module provides migration classes for performing migrations from a Ding/Drupal 6 site to a Ding2/Drupal 7 site.


Dependencies
------------

This module depends on the [http://drupal.org/project/migrate](migrate), [http://drupal.org/project/migrate_extras](migrate_extras) and [http://drupal.org/project/migrate_d2d](migrate_d2d) modules.
Included is ding_migrate_extras, which contain Field Handlers needed by migrate_ding1_ding2. These handlers might or might not be availaible depending on the ding2 installation, and so it can be necessary to enable ding_migrate_extras.
For example, MigrateLinkFieldHandler is included with Link 7.x-1.1, but at present, the ding2 distribution only includes Link 7.x-1.0. 

Installation
------------

Download and enable the module on an existing Ding2 installation. 


Configuration of the module
---------------------------

No UI configuration. All configuration is in code only. Go to /admin/content/migrate to view the state of the migration process.


Resources
---------

Migrations are complex, and they are handled purely through code - the Migrate UI is only a convenient overview during the process. Resources I have consulted include:

* [http://www.acquia.com/blog/drupal-drupal-data-migration-part-1-basics](http://www.acquia.com/blog/drupal-drupal-data-migration-part-1-basics)
* [http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture](http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture)
* [http://drupal.stackexchange.com/questions/50004/file-or-image-migrate-from-drupal-6-to-drupal-7-using-migrate-d2d-module](http://drupal.stackexchange.com/questions/50004/file-or-image-migrate-from-drupal-6-to-drupal-7-using-migrate-d2d-module)
* [http://www.grasmash.com/article/migrate-classes-location-cck-address-field](http://www.grasmash.com/article/migrate-classes-location-cck-address-field)
* [http://btmash.com/article/2011-02-25/migrating-content-using-migrate-module](http://btmash.com/article/2011-02-25/migrating-content-using-migrate-module)
* [http://dtek.net/blog/drupal-6-to-drupal-7-via-migrate-2](http://dtek.net/blog/drupal-6-to-drupal-7-via-migrate-2)

