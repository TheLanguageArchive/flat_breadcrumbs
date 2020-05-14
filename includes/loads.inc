<?php
/**
 * This file includes all the classes needed to run this module
 */
module_load_include('inc', 'flat_breadcrumbs', 'includes/exceptions/base');
module_load_include('inc', 'flat_breadcrumbs', 'includes/exceptions/already_fetched');
module_load_include('inc', 'flat_breadcrumbs', 'includes/exceptions/invalid_query');
module_load_include('inc', 'flat_breadcrumbs', 'includes/exceptions/not_found');
module_load_include('inc', 'flat_breadcrumbs', 'includes/backends/solr');
module_load_include('inc', 'flat_breadcrumbs', 'includes/db');
module_load_include('inc', 'flat_breadcrumbs', 'includes/breadcrumbs');
