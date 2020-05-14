<?php
/**
 * Getting root pid menu title
 *
 * @return string
 */
function flat_breadcrumbs_db_get_root() {

    $pid  = variable_get('islandora_repository_pid', 'islandora:root');
    $mlid = db_select('menu_links', 'ml')
        ->condition('ml.link_path', 'islandora')
        ->fields('ml', array('mlid'))
        ->execute()
        ->fetchField();

    $title = t('Home');

    if ($mlid) {
        $title = (isset($link['title']) ? $link['title'] : $title);
    }

    return [

        'pid'   => $pid,
        'title' => $title,
        'link'  => l($title, 'islandora'),
    ];
}
