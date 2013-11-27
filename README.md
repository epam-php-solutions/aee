ABOUT DEPLOY EXTRA
------------------

Extends the Deploy module functionality with the following:

Features:
---------
* Advanced logging system
* Test mode for deploy process
* Import/export to a file and form
* Media assets export and import with delayed import support
* Drush support for import/export operations
* Node revisions dependencies handling
* Node language dependencies handling
* Import materials from hook_update_N function

Pluggable architecture to:
--------------------------
* Support entities and configurations from contributed modules, such as Media, Entity Collection, Workbench Moderation and so forth
* Provide and consume the data in different formats, e.g. encrypt the data or export  in YAML

Powerful UI:
------------
* Bulk operations for entities
* Instant export with contextual links
* Ability to see dependencies before deploy process has been started
