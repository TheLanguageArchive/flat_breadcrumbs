<?php
/**
 * Disabling islandora breadcrumbs backend
 *
 * @param AbstractObject
 * @return array
 */
function flat_breadcrumbs_solr_backend(AbstractObject $object) {
    return [];
}
