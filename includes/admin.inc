<?php
/**
 * FLAT Breadcrumbs Admin Form
 *
 * @param array $form
 * @param array $form_state
 *
 * @return array
 */
function flat_breadcrumbs_admin_form($form, &$form_state) {

    return [

        'general' => [

            '#type'      => 'fieldset',
            '#title'     => t('FLAT Breadcrumbs'),
            'home_label' => [

                '#type'          => 'textfield',
                '#title'         => t('Home label'),
                '#size'          => 30,
                '#default_value' => variable_get('flat_breadcrumbs_home_label', FLAT_BREADCRUMBS_DEFAULT_HOME_LABEL),
            ],

            'exclusion_list' => [

                '#type'          => 'textarea',
                '#title'         => t('Exclusion list'),
                '#help_text'     => t("Please provide a list of Fedora PID's, separated by line delimeter, to exclude from breadcrumbs"),
                '#size'          => 30,
                '#default_value' => variable_get('flat_breadcrumbs_exclusion_list'),
            ],
        ],

        'actions' => [

            '#type'  => 'actions',
            'submit' => [

                '#type'   => 'submit',
                '#value'  => t('Save Configuration'),
                '#weight' => 0,
                '#submit' => ['flat_breadcrumbs_admin_form_submit'],
            ],

            'reset' => [

                '#type'   => 'submit',
                '#value'  => t('Reset to defaults'),
                '#weight' => 1,
                '#submit' => ['flat_breadcrumbs_admin_form_reset'],
            ],
        ]
    ];

    return $form;
}

/**
 * Admin form submit handler
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function flat_breadcrumbs_admin_form_submit($form, &$form_state) {

    variable_set('flat_breadcrumbs_home_label', $form_state['values']['home_label']);
    variable_set('flat_breadcrumbs_exclusion_list', trim($form_state['values']['exclusion_list']));

    drupal_set_message(t('FLAT Breadcrumbs admin configuration was successfully saved'));
}

/**
 * Admin form reset handler
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function flat_breadcrumbs_admin_form_reset($form, &$form_state) {

    variable_set('flat_breadcrumbs_home_label', FLAT_BREADCRUMBS_DEFAULT_HOME_LABEL);
    variable_set('flat_breadcrumbs_exclusion_list', '');

    drupal_set_message(t('FLAT Breadcrumbs admin configuration was successfully reset to defaults'));
}
