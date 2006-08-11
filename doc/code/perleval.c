
#include <EXTERN.h>               /* from the Perl distribution     */
#include <perl.h>                 /* from the Perl distribution     */

static PerlInterpreter *my_perl;  /***    The Perl interpreter    ***/

int main(int argc, char **argv, char **env)
{
   char *embedding[] = { "", "-e", "0" };

   PERL_SYS_INIT3(&argc,&argv,&env);
   my_perl = perl_alloc();
   perl_construct(my_perl);
   PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
   perl_parse (my_perl, NULL, 3, embedding, (char **)NULL);

   Perl_eval_pv(my_perl, " print \"Hello!\\n\"; ", TRUE);

   {
      int i;
      /* Cria um novo array. */
      AV* a = get_av("a", TRUE);
      /* Armazena dois valores, 0 e 1, nas primeiras posições do array. */
      av_push(a, newSViv(0));
      av_push(a, newSViv(1));
      for (i = 2; i < 10; i++) {
        /* Obviamente seria mais eficiente armazenar os valores em variáveis
            temporárias em C, mas obtenhamos os dois últimos valores da
            seqüência de volta do array Perl para fins de ilustração: */
         SV** penultimo_sv = av_fetch(a, i-2, FALSE);
         SV** ultimo_sv = av_fetch(a, i-1, FALSE);
         /* Obtém os inteiros armazenados nos SVs */
         int penultimo = SvIV(*penultimo_sv);
         int ultimo = SvIV(*ultimo_sv);
         /* Cria um novo SV e o insere no final do array */
         av_push(a, newSViv( penultimo + ultimo ) );
      }
   }

   Perl_eval_pv(my_perl, " for(@a) { "
                         "    print $_ . \"\\n\"; "
                         " } "
                         " sub foo { "
                         "    die "
                         " } "
                         , TRUE);

   perl_destruct(my_perl);
   perl_free(my_perl);
   PERL_SYS_TERM();
}

