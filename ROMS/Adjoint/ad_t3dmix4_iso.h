      SUBROUTINE ad_t3dmix4 (ng, tile)
!
!svn $Id: ad_t3dmix4_iso.h 937 2019-01-28 06:13:04Z arango $
!************************************************** Hernan G. Arango ***
!  Copyright (c) 2002-2020 The ROMS/TOMS Group       Andrew M. Moore   !
!    Licensed under a MIT/X style license                              !
!    See License_ROMS.txt                                              !
!***********************************************************************
!                                                                      !
!  This subroutine computes adjoint horizontal biharmonic mixing of    !
!  tracers along isopycnic surfaces.                                   !
!                                                                      !
!  BASIC STATE variables needed: diff4, Hz, t, rho, z_r                !
!                                                                      !
!***********************************************************************
!
      USE mod_param
#ifdef TS_MIX_CLIMA
      USE mod_clima
#endif
#ifdef DIAGNOSTICS_TS
!!    USE mod_diags
#endif
      USE mod_grid
      USE mod_mixing
      USE mod_ocean
      USE mod_stepping
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile
!
!  Local variable declarations.
!
      character (len=*), parameter :: MyFile =                          &
     &  __FILE__
!
#include "tile.h"
!
#ifdef PROFILE
      CALL wclock_on (ng, iADM, 29, __LINE__, MyFile)
#endif
      CALL ad_t3dmix4_tile (ng, tile,                                   &
     &                      LBi, UBi, LBj, UBj,                         &
     &                      IminS, ImaxS, JminS, JmaxS,                 &
     &                      nrhs(ng), nstp(ng), nnew(ng),               &
#ifdef MASKING
     &                      GRID(ng) % umask,                           &
     &                      GRID(ng) % vmask,                           &
#endif
#ifdef WET_DRY_NOT_YET
     &                      GRID(ng) % umask_wet,                       &
     &                      GRID(ng) % vmask_wet,                       &
#endif
     &                      GRID(ng) % om_v,                            &
     &                      GRID(ng) % on_u,                            &
     &                      GRID(ng) % pm,                              &
     &                      GRID(ng) % pn,                              &
     &                      GRID(ng) % Hz,                              &
     &                      GRID(ng) % ad_Hz,                           &
     &                      GRID(ng) % z_r,                             &
     &                      GRID(ng) % ad_z_r,                          &
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
     &                      MIXING(ng) % diff3d_u,                      &
     &                      MIXING(ng) % diff3d_v,                      &
# else
     &                      MIXING(ng) % diff3d_r,                      &
# endif
#else
     &                      MIXING(ng) % diff4,                         &
#endif
     &                      OCEAN(ng) % pden,                           &
     &                      OCEAN(ng) % ad_pden,                        &
#ifdef TS_MIX_CLIMA
     &                      CLIMA(ng) % tclm,                           &
#endif
#ifdef DIAGNOSTICS_TS
!!   &                      DIAGS(ng) % DiaTwrk,                        &
#endif
     &                      OCEAN(ng) % t,                              &
     &                      OCEAN(ng) % ad_t)
#ifdef PROFILE
      CALL wclock_off (ng, iADM, 29, __LINE__, MyFile)
#endif
!
      RETURN
      END SUBROUTINE ad_t3dmix4
!
!***********************************************************************
      SUBROUTINE ad_t3dmix4_tile (ng, tile,                             &
     &                            LBi, UBi, LBj, UBj,                   &
     &                            IminS, ImaxS, JminS, JmaxS,           &
     &                            nrhs, nstp, nnew,                     &
#ifdef MASKING
     &                            umask, vmask,                         &
#endif
#ifdef WET_DRY_NOT_YET
     &                            umask_wet, vmask_wet,                 &
#endif
     &                            om_v, on_u, pm, pn,                   &
     &                            Hz, ad_Hz,                            &
     &                            z_r, ad_z_r,                          &
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
     &                            diff3d_u, diff3d_v,                   &
# else
     &                            diff3d_r,                             &
# endif
#else
     &                            diff4,                                &
#endif
     &                            pden, ad_pden,                        &
#ifdef TS_MIX_CLIMA
     &                            tclm,                                 &
#endif
#ifdef DIAGNOSTICS_TS
!!   &                            DiaTwrk,                              &
#endif
     &                            t, ad_t)
!***********************************************************************
!
      USE mod_param
      USE mod_ncparam
      USE mod_scalars
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile
      integer, intent(in) :: LBi, UBi, LBj, UBj
      integer, intent(in) :: IminS, ImaxS, JminS, JmaxS
      integer, intent(in) :: nrhs, nstp, nnew

#ifdef ASSUMED_SHAPE
# ifdef MASKING
      real(r8), intent(in) :: umask(LBi:,LBj:)
      real(r8), intent(in) :: vmask(LBi:,LBj:)
# endif
# ifdef WET_DRY_NOT_YET
      real(r8), intent(in) :: umask_wet(LBi:,LBj:)
      real(r8), intent(in) :: vmask_wet(LBi:,LBj:)
# endif
# ifdef DIFF_3DCOEF
#  ifdef TS_U3ADV_SPLIT
      real(r8), intent(in) :: diff3d_u(LBi:,LBj:,:)
      real(r8), intent(in) :: diff3d_v(LBi:,LBj:,:)
#  else
      real(r8), intent(in) :: diff3d_r(LBi:,LBj:,:)
#  endif
# else
      real(r8), intent(in) :: diff4(LBi:,LBj:,:)
# endif
      real(r8), intent(in) :: om_v(LBi:,LBj:)
      real(r8), intent(in) :: on_u(LBi:,LBj:)
      real(r8), intent(in) :: pm(LBi:,LBj:)
      real(r8), intent(in) :: pn(LBi:,LBj:)
      real(r8), intent(in) :: Hz(LBi:,LBj:,:)
      real(r8), intent(in) :: z_r(LBi:,LBj:,:)
      real(r8), intent(in) :: pden(LBi:,LBj:,:)
      real(r8), intent(in) :: t(LBi:,LBj:,:,:,:)
# ifdef TS_MIX_CLIMA
      real(r8), intent(in) :: tclm(LBi:,LBj:,:,:)
# endif
# ifdef DIAGNOSTICS_TS
      real(r8), intent(inout) :: DiaTwrk(LBi:,LBj:,:,:,:)
# endif
      real(r8), intent(inout) :: ad_Hz(LBi:,LBj:,:)
      real(r8), intent(inout) :: ad_z_r(LBi:,LBj:,:)
      real(r8), intent(inout) :: ad_pden(LBi:,LBj:,:)
      real(r8), intent(inout) :: ad_t(LBi:,LBj:,:,:,:)
#else
# ifdef MASKING
      real(r8), intent(in) :: umask(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: vmask(LBi:UBi,LBj:UBj)
# endif
# ifdef WET_DRY_NOT_YET
      real(r8), intent(in) :: umask_wet(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: vmask_wet(LBi:UBi,LBj:UBj)
# endif
# ifdef DIFF_3DCOEF
#  ifdef TS_U3ADV_SPLIT
      real(r8), intent(in) :: diff3d_u(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: diff3d_v(LBi:UBi,LBj:UBj,N(ng))
#  else
      real(r8), intent(in) :: diff3d_r(LBi:UBi,LBj:UBj,N(ng))
#  endif
# else
      real(r8), intent(in) :: diff4(LBi:UBi,LBj:UBj,NT(ng))
# endif
      real(r8), intent(in) :: om_v(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: on_u(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pm(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pn(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: Hz(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: z_r(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: pden(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: t(LBi:UBi,LBj:UBj,N(ng),3,NT(ng))
# ifdef TS_MIX_CLIMA
      real(r8), intent(in) :: tclm(LBi:UBi,LBj:UBj,N(ng),NT(ng))
# endif
# ifdef DIAGNOSTICS_TS
!!    real(r8), intent(inout) :: DiaTwrk(LBi:UBi,LBj:UBj,N(ng),NT(ng),  &
!!   &                                   NDT)
# endif
      real(r8), intent(inout) :: ad_Hz(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(inout) :: ad_z_r(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(inout) :: ad_pden(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(inout) :: ad_t(LBi:UBi,LBj:UBj,N(ng),3,NT(ng))
#endif
!
!  Local variable declarations.
!
      integer :: Imin, Imax, Jmin, Jmax
      integer :: i, itrc, j, k, kk, kt, k1, k1b, k2, k2b

      real(r8), parameter :: eps = 0.5_r8
      real(r8), parameter :: small = 1.0E-14_r8
      real(r8), parameter :: slope_max = 0.0001_r8
      real(r8), parameter :: strat_min = 0.1_r8

      real(r8) :: cff, cff1, cff2, cff3, cff4, dife, difx
      real(r8) :: ad_cff, ad_cff1, ad_cff2, ad_cff3, ad_cff4
      real(r8) :: adfac, adfac1, adfac2, adfac3, adfac4

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,N(ng)) :: LapT

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,N(ng)) :: ad_LapT

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: FE
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: FX

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: ad_FE
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: ad_FX

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: FS
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: FS1
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: dRde
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: dRdx
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: dTde
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: dTdr
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: dTdx

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: ad_FS
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: ad_dRde
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: ad_dRdx
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: ad_dTde
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: ad_dTdr
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS,2) :: ad_dTdx

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Initialize adjoint private variables.
!-----------------------------------------------------------------------
!
      ad_cff=0.0_r8
      ad_cff1=0.0_r8
      ad_cff2=0.0_r8
      ad_cff3=0.0_r8
      ad_cff4=0.0_r8

      ad_FE(IminS:ImaxS,JminS:JmaxS)=0.0_r8
      ad_FX(IminS:ImaxS,JminS:JmaxS)=0.0_r8

      ad_FS(IminS:ImaxS,JminS:JmaxS,1:2)=0.0_r8

      ad_dRde(IminS:ImaxS,JminS:JmaxS,1:2)=0.0_r8
      ad_dRdx(IminS:ImaxS,JminS:JmaxS,1:2)=0.0_r8
      ad_dTde(IminS:ImaxS,JminS:JmaxS,1:2)=0.0_r8
      ad_dTdr(IminS:ImaxS,JminS:JmaxS,1:2)=0.0_r8
      ad_dTdx(IminS:ImaxS,JminS:JmaxS,1:2)=0.0_r8

      ad_LapT(IminS:ImaxS,JminS:JmaxS,1:N(ng))=0.0_r8
!
!----------------------------------------------------------------------
!  Compute horizontal biharmonic diffusion along isopycnic surfaces.
!  The biharmonic operator is computed by applying the harmonic
!  operator twice.
!----------------------------------------------------------------------
!
!  Set local I- and J-ranges.
!
      IF (EWperiodic(ng)) THEN
        Imin=Istr-1
        Imax=Iend+1
      ELSE
        Imin=MAX(Istr-1,1)
        Imax=MIN(Iend+1,Lm(ng))
      END IF
      IF (NSperiodic(ng)) THEN
        Jmin=Jstr-1
        Jmax=Jend+1
      ELSE
        Jmin=MAX(Jstr-1,1)
        Jmax=MIN(Jend+1,Mm(ng))
      END IF
!
!  Compute horizontal and density gradients for the BASIC STATE. Notice
!  the recursive blocking sequence. The vertical placement of the
!  gradients is:
!
!        dTdx,dTde(:,:,k1) k     rho-points
!        dTdx,dTde(:,:,k2) k+1   rho-points
!          FS,dTdr(:,:,k1) k-1/2   W-points
!          FS,dTdr(:,:,k2) k+1/2   W-points
!
#ifdef TS_MIX_STABILITY
!  In order to increase stability, the biharmonic operator is applied
!  as: 3/4 t(:,:,:,nrhs,:) + 1/4 t(:,:,:,nstp,:).
!
#endif

      T_LOOP : DO itrc=1,NT(ng)
        k2=1
        K_LOOP1 : DO k=0,N(ng)
          k1=k2
          k2=3-k1
          IF (k.lt.N(ng)) THEN
            DO j=Jmin,Jmax
              DO i=Imin,Imax+1
                cff=0.5_r8*(pm(i,j)+pm(i-1,j))
#ifdef MASKING
                cff=cff*umask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                cff=cff*umask_wet(i,j)
#endif
                dRdx(i,j,k2)=cff*(pden(i  ,j,k+1)-                      &
     &                            pden(i-1,j,k+1))
#if defined TS_MIX_STABILITY
                dTdx(i,j,k2)=cff*(0.75_r8*(t(i  ,j,k+1,nrhs,itrc)-      &
     &                                     t(i-1,j,k+1,nrhs,itrc))+     &
     &                            0.25_r8*(t(i  ,j,k+1,nstp,itrc)-      &
     &                                     t(i-1,j,k+1,nstp,itrc)))
#elif defined TS_MIX_CLIMA
                IF (LtracerCLM(itrc,ng)) THEN
                  dTdx(i,j,k2)=cff*((t(i  ,j,k+1,nrhs,itrc)-            &
     &                               tclm(i  ,j,k+1,itrc))-             &
     &                              (t(i-1,j,k+1,nrhs,itrc)-            &
     &                               tclm(i-1,j,k+1,itrc)))
                ELSE
                  dTdx(i,j,k2)=cff*(t(i  ,j,k+1,nrhs,itrc)-             &
     &                              t(i-1,j,k+1,nrhs,itrc))
                END IF
#else
                dTdx(i,j,k2)=cff*(t(i  ,j,k+1,nrhs,itrc)-               &
     &                            t(i-1,j,k+1,nrhs,itrc))
#endif
              END DO
            END DO
            DO j=Jmin,Jmax+1
              DO i=Imin,Imax
                cff=0.5_r8*(pn(i,j)+pn(i,j-1))
#ifdef MASKING
                cff=cff*vmask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                cff=cff*vmask_wet(i,j)
#endif
                dRde(i,j,k2)=cff*(pden(i,j  ,k+1)-                      &
     &                            pden(i,j-1,k+1))
#if defined TS_MIX_STABILITY
                dTde(i,j,k2)=cff*(0.75_r8*(t(i,j  ,k+1,nrhs,itrc)-      &
     &                                     t(i,j-1,k+1,nrhs,itrc))+     &
     &                            0.25_r8*(t(i,j  ,k+1,nstp,itrc)-      &
     &                                     t(i,j-1,k+1,nstp,itrc)))
#elif defined TS_MIX_CLIMA
                IF (LtracerCLM(itrc,ng)) THEN
                  dTde(i,j,k2)=cff*((t(i,j  ,k+1,nrhs,itrc)-            &
     &                               tclm(i,j  ,k+1,itrc))-             &
     &                              (t(i,j-1,k+1,nrhs,itrc)-            &
     &                               tclm(i,j-1,k+1,itrc)))
                ELSE
                  dTde(i,j,k2)=cff*(t(i,j  ,k+1,nrhs,itrc)-             &
     &                              t(i,j-1,k+1,nrhs,itrc))
                END IF
#else
                dTde(i,j,k2)=cff*(t(i,j  ,k+1,nrhs,itrc)-               &
     &                            t(i,j-1,k+1,nrhs,itrc))
#endif
              END DO
            END DO
          END IF
          IF ((k.eq.0).or.(k.eq.N(ng))) THEN
            DO j=-1+Jmin,Jmax+1
              DO i=-1+Imin,Imax+1
                dTdr(i,j,k2)=0.0_r8
                FS(i,j,k2)=0.0_r8
              END DO
            END DO
          ELSE
            DO j=-1+Jmin,Jmax+1
              DO i=-1+Imin,Imax+1
#if defined TS_MIX_MAX_SLOPE
                cff1=SQRT(dRdx(i,j,k2)**2+dRdx(i+1,j,k2)**2+            &
     &                    dRdx(i,j,k1)**2+dRdx(i+1,j,k1)**2+            &
     &                    dRde(i,j,k2)**2+dRde(i,j+1,k2)**2+            &
     &                    dRde(i,j,k1)**2+dRde(i,j+1,k1)**2)
                cff2=0.25_r8*slope_max*                                 &
     &               (z_r(i,j,k+1)-z_r(i,j,k))*cff1
                cff3=MAX(pden(i,j,k)-pden(i,j,k+1),small)
                cff4=MAX(cff2,cff3)
                cff=-1.0_r8/cff4
#elif defined TS_MIX_MIN_STRAT
                cff1=MAX(pden(i,j,k)-pden(i,j,k+1),                     &
     &                   strat_min*(z_r(i,j,k+1)-z_r(i,j,k)))
                cff=-1.0_r8/cff1
#else
                cff1=MAX(pden(i,j,k)-pden(i,j,k+1),eps)
                cff=-1.0_r8/cff1
#endif
#if defined TS_MIX_STABILITY
                dTdr(i,j,k2)=cff*(0.75_r8*(t(i,j,k+1,nrhs,itrc)-        &
     &                                     t(i,j,k  ,nrhs,itrc))+       &
     &                            0.25_r8*(t(i,j,k+1,nstp,itrc)-        &
     &                                     t(i,j,k  ,nstp,itrc)))
#elif defined TS_MIX_CLIMA
                IF (LtracerCLM(itrc,ng)) THEN
                  dTdr(i,j,k2)=cff*((t(i,j,k+1,nrhs,itrc)-              &
     &                               tclm(i,j,k+1,itrc))-               &
     &                              (t(i,j,k  ,nrhs,itrc)-              &
     &                               tclm(i,j,k  ,itrc)))
                ELSE
                  dTdr(i,j,k2)=cff*(t(i,j,k+1,nrhs,itrc)-               &
     &                              t(i,j,k  ,nrhs,itrc))
                END IF
#else
                dTdr(i,j,k2)=cff*(t(i,j,k+1,nrhs,itrc)-                 &
     &                            t(i,j,k  ,nrhs,itrc))
#endif
                FS(i,j,k2)=cff*(z_r(i,j,k+1)-                           &
     &                          z_r(i,j,k  ))
              END DO
            END DO
          END IF
          IF (k.gt.0) THEN
            DO j=Jmin,Jmax
              DO i=Imin,Imax+1
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                cff=0.5_r8*diff3d_u(i,j,k)*on_u(i,j)
# else
                cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i-1,j,k))*        &
     &              on_u(i,j)
# endif
#else
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i-1,j,itrc))*        &
     &              on_u(i,j)
#endif
                FX(i,j)=cff*                                            &
     &                  (Hz(i,j,k)+Hz(i-1,j,k))*                        &
     &                  (dTdx(i,j,k1)-                                  &
     &                   0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i-1,j,k1)+                    &
     &                               dTdr(i  ,j,k2))+                   &
     &                           MIN(dRdx(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i-1,j,k2)+                    &
     &                               dTdr(i  ,j,k1))))
              END DO
            END DO
            DO j=Jmin,Jmax+1
              DO i=Imin,Imax
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                cff=0.5_r8*diff3d_v(i,j,k)*om_v(i,j)
# else
                cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i,j-1,k))*        &
     &              om_v(i,j)
# endif
#else
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i,j-1,itrc))*        &
     &              om_v(i,j)
#endif
                FE(i,j)=cff*                                            &
     &                  (Hz(i,j,k)+Hz(i,j-1,k))*                        &
     &                  (dTde(i,j,k1)-                                  &
     &                   0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i,j-1,k1)+                    &
     &                               dTdr(i,j  ,k2))+                   &
     &                           MIN(dRde(i,j,k1),0.0_r8)*              &
     &                              (dTdr(i,j-1,k2)+                    &
     &                               dTdr(i,j  ,k1))))
              END DO
            END DO
            IF (k.lt.N(ng)) THEN
              DO j=Jmin,Jmax
                DO i=Imin,Imax
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                  difx=0.125_r8*(diff3d_u(i,j,k  )+diff3d_u(i+1,j,k  )+ &
     &                           diff3d_u(i,j,k+1)+diff3d_u(i+1,j,k+1))
                  dife=0.125_r8*(diff3d_v(i,j,k  )+diff3d_v(i,j+1,k  )+ &
     &                           diff3d_v(i,j,k+1)+diff3d_v(i,j+1,k+1))
# else
                  difx=0.5_r8*diff3d_r(i,j,k)
                  dife=difx
# endif
#else
                  difx=0.5_r8*diff4(i,j,itrc)
                  dife=difx
#endif
                  cff1=MAX(dRdx(i  ,j,k1),0.0_r8)
                  cff2=MAX(dRdx(i+1,j,k2),0.0_r8)
                  cff3=MIN(dRdx(i  ,j,k2),0.0_r8)
                  cff4=MIN(dRdx(i+1,j,k1),0.0_r8)
                  cff=difx*                                             &
     &                (cff1*(cff1*dTdr(i,j,k2)-dTdx(i  ,j,k1))+         &
     &                 cff2*(cff2*dTdr(i,j,k2)-dTdx(i+1,j,k2))+         &
     &                 cff3*(cff3*dTdr(i,j,k2)-dTdx(i  ,j,k2))+         &
     &                 cff4*(cff4*dTdr(i,j,k2)-dTdx(i+1,j,k1)))
                  cff1=MAX(dRde(i,j  ,k1),0.0_r8)
                  cff2=MAX(dRde(i,j+1,k2),0.0_r8)
                  cff3=MIN(dRde(i,j  ,k2),0.0_r8)
                  cff4=MIN(dRde(i,j+1,k1),0.0_r8)
                  cff=cff+                                              &
     &                dife*                                             &
     &                (cff1*(cff1*dTdr(i,j,k2)-dTde(i,j  ,k1))+         &
     &                 cff2*(cff2*dTdr(i,j,k2)-dTde(i,j+1,k2))+         &
     &                 cff3*(cff3*dTdr(i,j,k2)-dTde(i,j  ,k2))+         &
     &                 cff4*(cff4*dTdr(i,j,k2)-dTde(i,j+1,k1)))
                  FS(i,j,k2)=cff*FS(i,j,k2)
                END DO
              END DO
            END IF
!
!  Compute first BASIC STATE harmonic operator, without mixing
!  coefficient. Multiply by the metrics of the second harmonic
!  operator.  Save into work array "LapT".
!
            DO j=Jmin,Jmax
              DO i=Imin,Imax
                cff=pm(i,j)*pn(i,j)
                cff1=1.0_r8/Hz(i,j,k)
                LapT(i,j,k)=cff1*(cff*                                  &
     &                            (FX(i+1,j)-FX(i,j)+                   &
     &                             FE(i,j+1)-FE(i,j))+                  &
     &                            (FS(i,j,k2)-FS(i,j,k1)))
              END DO
            END DO
          END IF
        END DO K_LOOP1
!
!  Apply boundary conditions (except periodic; closed or gradient)
!  to the first BASIC STATE harmonic operator.
!
        IF (.not.(CompositeGrid(iwest,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%Western_Edge(tile)) THEN
            IF (ad_LBC(iwest,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO j=Jmin,Jmax
                  LapT(Istr-1,j,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO j=Jmin,Jmax
                  LapT(Istr-1,j,k)=LapT(Istr,j,k)
                END DO
              END DO
            END IF
          END IF
        END IF
!
        IF (.not.(CompositeGrid(ieast,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%Eastern_Edge(tile)) THEN
            IF (ad_LBC(ieast,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO j=Jmin,Jmax
                  LapT(Iend+1,j,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO j=Jmin,Jmax
                  LapT(Iend+1,j,k)=LapT(Iend,j,k)
                END DO
              END DO
            END IF
          END IF
        END IF
!
        IF (.not.(CompositeGrid(isouth,ng).or.NSperiodic(ng))) THEN
          IF (DOMAIN(ng)%Southern_Edge(tile)) THEN
            IF (ad_LBC(isouth,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO i=Imin,Imax
                  LapT(i,Jstr-1,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO i=Imin,Imax
                  LapT(i,Jstr-1,k)=LapT(i,Jstr,k)
                END DO
              END DO
            END IF
          END IF
        END IF
!
        IF (.not.(CompositeGrid(inorth,ng).or.NSperiodic(ng))) THEN
          IF (DOMAIN(ng)%Northern_Edge(tile)) THEN
            IF (ad_LBC(inorth,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO i=Imin,Imax
                  LapT(i,Jend+1,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO i=Imin,Imax
                  LapT(i,Jend+1,k)=LapT(i,Jend,k)
                END DO
              END DO
            END IF
          END IF
        END IF
!
        IF (.not.(CompositeGrid(isouth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(iwest ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%SouthWest_Corner(tile)) THEN
            DO k=1,N(ng)
              LapT(Istr-1,Jstr-1,k)=0.5_r8*                             &
     &                              (LapT(Istr  ,Jstr-1,k)+             &
     &                               LapT(Istr-1,Jstr  ,k))
            END DO
          END IF
        END IF

        IF (.not.(CompositeGrid(isouth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(ieast ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%SouthEast_Corner(tile)) THEN
            DO k=1,N(ng)
              LapT(Iend+1,Jstr-1,k)=0.5_r8*                             &
     &                              (LapT(Iend  ,Jstr-1,k)+             &
     &                               LapT(Iend+1,Jstr  ,k))
            END DO
          END IF
        END IF

        IF (.not.(CompositeGrid(inorth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(iwest ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%NorthWest_Corner(tile)) THEN
            DO k=1,N(ng)
              LapT(Istr-1,Jend+1,k)=0.5_r8*                             &
     &                              (LapT(Istr  ,Jend+1,k)+             &
     &                               LapT(Istr-1,Jend  ,k))
            END DO
          END IF
        END IF

        IF (.not.(CompositeGrid(inorth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(ieast ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%NorthEast_Corner(tile)) THEN
            DO k=1,N(ng)
              LapT(Iend+1,Jend+1,k)=0.5_r8*                             &
     &                              (LapT(Iend  ,Jend+1,k)+             &
     &                               LapT(Iend+1,Jend  ,k))
            END DO
          END IF
        END IF
!
! Compute adjoint of starting storage recursive indices k1 and k2.
!
        k1=2
        k2=1
        DO k=0,N(ng)
!!
!!  Note: The following code is equivalent to
!!
!!        kt=k1
!!        k1=k2
!!        k2=kt
!!
!!  We use the adjoint of the above code.
!!
          k1=k2
          k2=3-k1
        END DO
!
!  Compute required basic state fields. Need to look forward in
!  recursive kk index.
!
        K_LOOP2: DO k=N(ng),0,-1
          k2b=1
          DO kk=0,k
            k1b=k2b
            k2b=3-k1b
!
!  Compute components of the rotated tracer flux (T m3/s) along
!  isopycnic surfaces (required BASIC STATE fields).
!
            IF (kk.lt.N(ng)) THEN
              DO j=Jstr,Jend
                DO i=Istr,Iend+1
                  cff=0.5_r8*(pm(i,j)+pm(i-1,j))
#ifdef MASKING
                  cff=cff*umask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                  cff=cff*umask_wet(i,j)
#endif
                  dRdx(i,j,k2b)=cff*(pden(i  ,j,kk+1)-                  &
     &                               pden(i-1,j,kk+1))
                  dTdx(i,j,k2b)=cff*(LapT(i  ,j,kk+1)-                  &
     &                               LapT(i-1,j,kk+1))
                END DO
              END DO
              IF (kk.eq.0) THEN
                DO j=Jstr,Jend
                  DO i=Istr,Iend+1
                    dRdx(i,j,k1b)=0.0_r8
                    dTdx(i,j,k1b)=0.0_r8
                  END DO
                END DO
              END IF
              DO j=Jstr,Jend+1
                DO i=Istr,Iend
                  cff=0.5_r8*(pn(i,j)+pn(i,j-1))
#ifdef MASKING
                  cff=cff*vmask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                  cff=cff*vmask_wet(i,j)
#endif
                  dRde(i,j,k2b)=cff*(pden(i,j  ,kk+1)-                  &
     &                               pden(i,j-1,kk+1))
                  dTde(i,j,k2b)=cff*(LapT(i,j  ,kk+1)-                  &
     &                               LapT(i,j-1,kk+1))
                END DO
              END DO
              IF (kk.eq.0) THEN
                DO j=Jstr,Jend+1
                  DO i=Istr,Iend
                    dRde(i,j,k1b)=0.0_r8
                    dTde(i,j,k1b)=0.0_r8
                  END DO
                END DO
              END IF
            END IF
            IF ((kk.eq.0).or.(kk.eq.N(ng))) THEN
              DO j=Jstr-1,Jend+1
                DO i=Istr-1,Iend+1
                  dTdr(i,j,k2b)=0.0_r8
                  FS(i,j,k2b)=0.0_r8
                END DO
              END DO
              IF (kk.eq.0) THEN
                DO j=Jstr-1,Jend+1
                  DO i=Istr-1,Iend+1
                    dTdr(i,j,k1b)=0.0_r8
                    FS(i,j,k1b)=0.0_r8
                  END DO
                END DO
              END IF
            ELSE
              DO j=Jstr-1,Jend+1
                DO i=Istr-1,Iend+1
#if defined TS_MIX_MAX_SLOPE
                  cff1=SQRT(dRdx(i,j,k2b)**2+dRdx(i+1,j,k2b)**2+        &
     &                      dRdx(i,j,k1b)**2+dRdx(i+1,j,k1b)**2+        &
     &                      dRde(i,j,k2b)**2+dRde(i,j+1,k2b)**2+        &
     &                      dRde(i,j,k1b)**2+dRde(i,j+1,k1b)**2)
                  cff2=0.25_r8*slope_max*                               &
     &                 (z_r(i,j,kk+1)-z_r(i,j,kk))*cff1
                  cff3=MAX(pden(i,j,kk)-pden(i,j,kk+1),small)
                  cff4=MAX(cff2,cff3)
                  cff=-1.0_r8/cff4
#elif defined TS_MIX_MIN_STRAT
                  cff1=MAX(pden(i,j,kk)-pden(i,j,kk+1),                 &
     &                     strat_min*(z_r(i,j,kk+1)-z_r(i,j,kk)))
                  cff=-1.0_r8/cff1
#else
                  cff1=MAX(pden(i,j,kk)-pden(i,j,kk+1),eps)
                  cff=-1.0_r8/cff1
#endif
                  dTdr(i,j,k2b)=cff*(LapT(i,j,kk+1)-                    &
     &                               LapT(i,j,kk  ))
                  FS(i,j,k2b)=cff*(z_r(i,j,kk+1)-                       &
     &                             z_r(i,j,kk  ))
                END DO
              END DO
            END IF
          END DO
!
          IF (k.gt.0) THEN
!
!  Time-step biharmonic, isopycnal diffusion term (m Tunits).
!
            DO j=Jstr,Jend
              DO i=Istr,Iend
#ifdef DIAGNOSTICS_TS
!!              DiaTwrk(i,j,k,itrc,iThdif)=-cff
#endif
!>              tl_t(i,j,k,nnew,itrc)=tl_t(i,j,k,nnew,itrc)-tl_cff
!>
                ad_cff=ad_cff-ad_t(i,j,k,nnew,itrc)
!>              tl_cff=dt(ng)*pm(i,j)*pn(i,j)*                          &
!>   &                        (tl_FX(i+1,j)-tl_FX(i,j)+                 &
!>   &                         tl_FE(i,j+1)-tl_FE(i,j))+                &
!>   &                 dt(ng)*(tl_FS(i,j,k2)-tl_FS(i,j,k1))
!>
                adfac=dt(ng)*ad_cff
                adfac1=adfac*pm(i,j)*pn(i,j)
                ad_FS(i,j,k1)=ad_FS(i,j,k1)-adfac
                ad_FS(i,j,k2)=ad_FS(i,j,k2)+adfac
                ad_FX(i  ,j)=ad_FX(i  ,j)-adfac1
                ad_FX(i+1,j)=ad_FX(i+1,j)+adfac1
                ad_FE(i,j  )=ad_FE(i,j  )-adfac1
                ad_FE(i,j+1)=ad_FE(i,j+1)+adfac1
                ad_cff=0.0_r8
              END DO
            END DO
            IF (k.lt.N(ng)) THEN
              DO j=Jstr,Jend
                DO i=Istr,Iend
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                  difx=0.125_r8*(diff3d_u(i,j,k  )+diff3d_u(i+1,j,k  )+ &
     &                           diff3d_u(i,j,k+1)+diff3d_u(i+1,j,k+1))
                  dife=0.125_r8*(diff3d_v(i,j,k  )+diff3d_v(i,j+1,k  )+ &
     &                           diff3d_v(i,j,k+1)+diff3d_v(i,j+1,k+1))
# else
                  difx=0.5_r8*diff3d_r(i,j,k)
                  dife=difx
# endif
#else
                  difx=0.5_r8*diff4(i,j,itrc)
                  dife=difx
#endif
                  cff1=MAX(dRdx(i  ,j,k1),0.0_r8)
                  cff2=MAX(dRdx(i+1,j,k2),0.0_r8)
                  cff3=MIN(dRdx(i  ,j,k2),0.0_r8)
                  cff4=MIN(dRdx(i+1,j,k1),0.0_r8)
                  cff=difx*                                             &
     &                (cff1*(cff1*dTdr(i,j,k2)-dTdx(i  ,j,k1))+         &
     &                 cff2*(cff2*dTdr(i,j,k2)-dTdx(i+1,j,k2))+         &
     &                 cff3*(cff3*dTdr(i,j,k2)-dTdx(i  ,j,k2))+         &
     &                 cff4*(cff4*dTdr(i,j,k2)-dTdx(i+1,j,k1)))
                  cff1=MAX(dRde(i,j  ,k1),0.0_r8)
                  cff2=MAX(dRde(i,j+1,k2),0.0_r8)
                  cff3=MIN(dRde(i,j  ,k2),0.0_r8)
                  cff4=MIN(dRde(i,j+1,k1),0.0_r8)
                  cff=cff+                                              &
     &                dife*                                             &
     &                (cff1*(cff1*dTdr(i,j,k2)-dTde(i,j  ,k1))+         &
     &                 cff2*(cff2*dTdr(i,j,k2)-dTde(i,j+1,k2))+         &
     &                 cff3*(cff3*dTdr(i,j,k2)-dTde(i,j  ,k2))+         &
     &                 cff4*(cff4*dTdr(i,j,k2)-dTde(i,j+1,k1)))
!>                tl_FS(i,j,k2)=tl_cff*FS(i,j,k2)+                      &
!>   &                          cff*tl_FS(i,j,k2
!>
                  ad_cff=ad_cff+FS(i,j,k2)*ad_FS(i,j,k2)
                  ad_FS(i,j,k2)=cff*ad_FS(i,j,k2)
!>                tl_cff=tl_cff+                                        &
!>   &                   dife*                                          &
!>   &                   (tl_cff1*(cff1*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j  ,k1))+                     &
!>   &                    tl_cff2*(cff2*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j+1,k2))+                     &
!>   &                    tl_cff3*(cff3*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j  ,k2))+                     &
!>   &                    tl_cff4*(cff4*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j+1,k1))+                     &
!>   &                    cff1*(tl_cff1*dTdr(i,j,k2)+                   &
!>   &                          cff1*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j  ,k1))+                     &
!>   &                    cff2*(tl_cff2*dTdr(i,j,k2)+                   &
!>   &                          cff2*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j+1,k2))+                     &
!>   &                    cff3*(tl_cff3*dTdr(i,j,k2)+                   &
!>   &                          cff3*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j  ,k2))+                     &
!>   &                    cff4*(tl_cff4*dTdr(i,j,k2)+                   &
!>   &                          cff4*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j+1,k1)))
!>
                  adfac=dife*ad_cff
                  ad_cff1=ad_cff1+                                      &
     &                    (2.0_r8*cff1*dTdr(i,j,k2)-dTde(i,j  ,k1))*    &
     &                    adfac
                  ad_cff2=ad_cff2+                                      &
     &                    (2.0_r8*cff2*dTdr(i,j,k2)-dTde(i,j+1,k2))*    &
     &                    adfac
                  ad_cff3=ad_cff3+                                      &
     &                    (2.0_r8*cff3*dTdr(i,j,k2)-dTde(i,j  ,k2))*    &
     &                    adfac
                  ad_cff4=ad_cff4+                                      &
     &                    (2.0_r8*cff4*dTdr(i,j,k2)-dTde(i,j+1,k1))*    &
     &                    adfac
                  ad_dTdr(i,j,k2)=ad_dTdr(i,j,k2)+                      &
     &                            (cff1*cff1+                           &
     &                             cff2*cff2+                           &
     &                             cff3*cff3+                           &
     &                             cff4*cff4)*adfac
                  ad_dTde(i,j  ,k1)=ad_dTde(i,j  ,k1)-cff1*adfac
                  ad_dTde(i,j+1,k2)=ad_dTde(i,j+1,k2)-cff2*adfac
                  ad_dTde(i,j  ,k2)=ad_dTde(i,j  ,k2)-cff3*adfac
                  ad_dTde(i,j+1,k1)=ad_dTde(i,j+1,k1)-cff4*adfac
!>                tl_cff4=(0.5_r8+SIGN(0.5_r8,-dRde(i,j+1,k1)))*        &
!>   &                    tl_dRde(i,j+1,k1)
!>
                  ad_dRde(i,j+1,k1)=ad_dRde(i,j+1,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRde(i,j+1,k1)))*     &
     &                              ad_cff4
                  ad_cff4=0.0_r8
!>                tl_cff3=(0.5_r8+SIGN(0.5_r8,-dRde(i,j  ,k2)))*        &
!>   &                    tl_dRde(i,j  ,k2)
!>
                  ad_dRde(i,j  ,k2)=ad_dRde(i,j  ,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRde(i,j  ,k2)))*     &
     &                              ad_cff3
                  ad_cff3=0.0_r8
!>                tl_cff2=(0.5_r8+SIGN(0.5_r8, dRde(i,j+1,k2)))*        &
!>   &                    tl_dRde(i,j+1,k2)
!>
                  ad_dRde(i,j+1,k2)=ad_dRde(i,j+1,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRde(i,j+1,k2)))*     &
     &                              ad_cff2
                  ad_cff2=0.0_r8
!>                tl_cff1=(0.5_r8+SIGN(0.5_r8, dRde(i,j  ,k1)))*        &
!>   &                    tl_dRde(i,j  ,k1)
!>
                  ad_dRde(i,j  ,k1)=ad_dRde(i,j  ,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRde(i,j  ,k1)))*     &
     &                              ad_cff1
                  ad_cff1=0.0_r8

                  cff1=MAX(dRdx(i  ,j,k1),0.0_r8)
                  cff2=MAX(dRdx(i+1,j,k2),0.0_r8)
                  cff3=MIN(dRdx(i  ,j,k2),0.0_r8)
                  cff4=MIN(dRdx(i+1,j,k1),0.0_r8)
!>                tl_cff=difx*                                          &
!>   &                   (tl_cff1*(cff1*dTdr(i,j,k2)-                   &
!>   &                             dTdx(i  ,j,k1))+                     &
!>   &                    tl_cff2*(cff2*dTdr(i,j,k2)-                   &
!>   &                             dTdx(i+1,j,k2))+                     &
!>   &                    tl_cff3*(cff3*dTdr(i,j,k2)-                   &
!>   &                             dTdx(i  ,j,k2))+                     &
!>   &                    tl_cff4*(cff4*dTdr(i,j,k2)-                   &
!>   &                             dTdx(i+1,j,k1))+                     &
!>   &                    cff1*(tl_cff1*dTdr(i,j,k2)+                   &
!>   &                          cff1*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i  ,j,k1))+                     &
!>   &                    cff2*(tl_cff2*dTdr(i,j,k2)+                   &
!>   &                          cff2*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i+1,j,k2))+                     &
!>   &                    cff3*(tl_cff3*dTdr(i,j,k2)+                   &
!>   &                          cff3*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i  ,j,k2))+                     &
!>   &                    cff4*(tl_cff4*dTdr(i,j,k2)+                   &
!>   &                          cff4*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i+1,j,k1)))
!>
                  adfac=difx*ad_cff
                  ad_cff1=ad_cff1+                                      &
     &                    (2.0_r8*cff1*dTdr(i,j,k2)-dTdx(i  ,j,k1))*    &
     &                    adfac
                  ad_cff2=ad_cff2+                                      &
     &                    (2.0_r8*cff2*dTdr(i,j,k2)-dTdx(i+1,j,k2))*    &
     &                    adfac
                  ad_cff3=ad_cff3+                                      &
     &                    (2.0_r8*cff3*dTdr(i,j,k2)-dTdx(i  ,j,k2))*    &
     &                    adfac
                  ad_cff4=ad_cff4+                                      &
     &                    (2.0_r8*cff4*dTdr(i,j,k2)-dTdx(i+1,j,k1))*    &
     &                    adfac
                  ad_dTdr(i,j,k2)=ad_dTdr(i,j,k2)+                      &
     &                            (cff1*cff1+                           &
     &                             cff2*cff2+                           &
     &                             cff3*cff3+                           &
     &                             cff4*cff4)*adfac
                  ad_dTdx(i  ,j,k1)=ad_dTdx(i  ,j,k1)-cff1*adfac
                  ad_dTdx(i+1,j,k2)=ad_dTdx(i+1,j,k2)-cff2*adfac
                  ad_dTdx(i  ,j,k2)=ad_dTdx(i  ,j,k2)-cff3*adfac
                  ad_dTdx(i+1,j,k1)=ad_dTdx(i+1,j,k1)-cff4*adfac
                  ad_cff=0.0_r8
!>                tl_cff4=(0.5_r8+SIGN(0.5_r8,-dRdx(i+1,j,k1)))*        &
!>   &                    tl_dRdx(i+1,j,k1)
!>
                  ad_dRdx(i+1,j,k1)=ad_dRdx(i+1,j,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRdx(i+1,j,k1)))*     &
     &                              ad_cff4
                  ad_cff4=0.0_r8
!>                tl_cff3=(0.5_r8+SIGN(0.5_r8,-dRdx(i  ,j,k2)))*        &
!>   &                    tl_dRdx(i  ,j,k2)
!>
                  ad_dRdx(i  ,j,k2)=ad_dRdx(i  ,j,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRdx(i  ,j,k2)))*     &
     &                              ad_cff3
                  ad_cff3=0.0_r8
!>                tl_cff2=(0.5_r8+SIGN(0.5_r8, dRdx(i+1,j,k2)))*        &
!>   &                    tl_dRdx(i+1,j,k2)
!>
                  ad_dRdx(i+1,j,k2)=ad_dRdx(i+1,j,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRdx(i+1,j,k2)))*     &
     &                              ad_cff2
                  ad_cff2=0.0_r8
!>                tl_cff1=(0.5_r8+SIGN(0.5_r8, dRdx(i  ,j,k1)))*        &
!>   &                    tl_dRdx(i  ,j,k1)
!>
                  ad_dRdx(i  ,j,k1)=ad_dRdx(i  ,j,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRdx(i  ,j,k1)))*     &
     &                              ad_cff1
                  ad_cff1=0.0_r8
                END DO
              END DO
            END IF
            DO j=Jstr,Jend+1
              DO i=Istr,Iend
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                cff=0.5_r8*diff3d_v(i,j,k)*om_v(i,j)
# else
                cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i,j-1,k))*        &
     &              om_v(i,j)
# endif
#else
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i,j-1,itrc))*        &
     &              om_v(i,j)
#endif
!>              tl_FE(i,j)=cff*                                         &
!>   &                     ((tl_Hz(i,j,k)+tl_Hz(i,j-1,k))*              &
!>   &                      (dTde(i,j,k1)-                              &
!>   &                       0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i,j-1,k1)+                &
!>   &                                   dTdr(i,j  ,k2))+               &
!>   &                               MIN(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i,j-1,k2)+                &
!>   &                                   dTdr(i,j  ,k1))))+             &
!>   &                      (Hz(i,j,k)+Hz(i,j-1,k))*                    &
!>   &                      (tl_dTde(i,j,k1)-                           &
!>   &                       0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i,j-1,k1)+             &
!>   &                                   tl_dTdr(i  ,j,k2))+            &
!>   &                               MIN(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i,j-1,k2)+             &
!>   &                                   tl_dTdr(i,j  ,k1)))-           &
!>   &                       0.5_r8*((0.5_r8+                           &
!>   &                                SIGN(0.5_r8, dRde(i,j,k1)))*      &
!>   &                               tl_dRde(i,j,k1)*                   &
!>   &                               (dTdr(i,j-1,k1)+dTdr(i,j,k2))+     &
!>   &                               (0.5_r8+                           &
!>   &                                SIGN(0.5_r8,-dRde(i,j,k1)))*      &
!>   &                               tl_dRde(i,j,k1)*                   &
!>   &                               (dTdr(i,j-1,k2)+dTdr(i,j,k1)))))
!>
                adfac=cff*ad_FE(i,j)
                adfac1=adfac*(dTde(i,j,k1)-                             &
     &                        0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i,j-1,k1)+               &
     &                                    dTdr(i,j  ,k2))+              &
     &                                MIN(dRde(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i,j-1,k2)+               &
     &                                    dTdr(i,j  ,k1))))
                adfac2=adfac*(Hz(i,j,k)+Hz(i,j-1,k))
                adfac3=adfac2*0.5_r8*MAX(dRde(i,j,k1),0.0_r8)
                adfac4=adfac2*0.5_r8*MIN(dRde(i,j,k1),0.0_r8)
                ad_Hz(i,j-1,k)=ad_Hz(i,j-1,k)+adfac1
                ad_Hz(i,j  ,k)=ad_Hz(i,j  ,k)+adfac1
                ad_dTde(i,j,k1)=ad_dTde(i,j,k1)+adfac2
                ad_dTdr(i,j-1,k1)=ad_dTdr(i,j-1,k1)-adfac3
                ad_dTdr(i,j  ,k2)=ad_dTdr(i,j  ,k2)-adfac3
                ad_dTdr(i,j-1,k2)=ad_dTdr(i,j-1,k2)-adfac4
                ad_dTdr(i,j  ,k1)=ad_dTdr(i,j  ,k1)-adfac4
                ad_dRde(i,j,k1)=ad_dRde(i,j,k1)-                        &
     &                          adfac2*0.5_r8*                          &
     &                          ((0.5_r8+SIGN(0.5_r8, dRde(i,j,k1)))*   &
     &                           (dTdr(i,j-1,k1)+dTdr(i,j,k2))+         &
     &                           (0.5_r8+SIGN(0.5_r8,-dRde(i,j,k1)))*   &
     &                           (dTdr(i,j-1,k2)+dTdr(i,j,k1)))
                ad_FE(i,j)=0.0_r8
              END DO
            END DO
            DO j=Jstr,Jend
              DO i=Istr,Iend+1
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                cff=0.5_r8*diff3d_u(i,j,k)*on_u(i,j)
# else
                cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i-1,j,k))*        &
     &              on_u(i,j)
# endif
#else
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i-1,j,itrc))*        &
     &              on_u(i,j)
#endif
!>              tl_FX(i,j)=cff*                                         &
!>   &                     ((tl_Hz(i,j,k)+tl_Hz(i-1,j,k))*              &
!>   &                      (dTdx(i,j,k1)-                              &
!>   &                       0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i-1,j,k1)+                &
!>   &                                   dTdr(i  ,j,k2))+               &
!>   &                               MIN(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i-1,j,k2)+                &
!>   &                                   dTdr(i  ,j,k1))))+             &
!>   &                      (Hz(i,j,k)+Hz(i-1,j,k))*                    &
!>   &                      (tl_dTdx(i,j,k1)-                           &
!>   &                       0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i-1,j,k1)+             &
!>   &                                   tl_dTdr(i  ,j,k2))+            &
!>   &                               MIN(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i-1,j,k2)+             &
!>   &                                   tl_dTdr(i  ,j,k1)))-           &
!>   &                       0.5_r8*((0.5_r8+                           &
!>   &                                SIGN(0.5_r8, dRdx(i,j,k1)))*      &
!>   &                               tl_dRdx(i,j,k1)*                   &
!>   &                               (dTdr(i-1,j,k1)+dTdr(i,j,k2))+     &
!>   &                               (0.5_r8+                           &
!>   &                                SIGN(0.5_r8,-dRdx(i,j,k1)))*      &
!>   &                               tl_dRdx(i,j,k1)*                   &
!>   &                               (dTdr(i-1,j,k2)+dTdr(i,j,k1)))))
!>
                adfac=cff*ad_FX(i,j)
                adfac1=adfac*(dTdx(i  ,j,k1)-                           &
     &                        0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i-1,j,k1)+               &
     &                                    dTdr(i  ,j,k2))+              &
     &                                MIN(dRdx(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i-1,j,k2)+               &
     &                                    dTdr(i  ,j,k1))))
                adfac2=adfac*(Hz(i,j,k)+Hz(i-1,j,k))
                adfac3=adfac2*0.5_r8*MAX(dRdx(i,j,k1),0.0_r8)
                adfac4=adfac2*0.5_r8*MIN(dRdx(i,j,k1),0.0_r8)
                ad_Hz(i-1,j,k)=ad_Hz(i-1,j,k)+adfac1
                ad_Hz(i  ,j,k)=ad_Hz(i  ,j,k)+adfac1
                ad_dTdx(i  ,j,k1)=ad_dTdx(i  ,j,k1)+adfac2
                ad_dTdr(i-1,j,k1)=ad_dTdr(i-1,j,k1)-adfac3
                ad_dTdr(i  ,j,k2)=ad_dTdr(i  ,j,k2)-adfac3
                ad_dTdr(i-1,j,k2)=ad_dTdr(i-1,j,k2)-adfac4
                ad_dTdr(i  ,j,k1)=ad_dTdr(i  ,j,k1)-adfac4
                ad_dRdx(i,j,k1)=ad_dRdx(i,j,k1)-                        &
     &                          adfac2*0.5_r8*                          &
     &                          ((0.5_r8+SIGN(0.5_r8, dRdx(i,j,k1)))*   &
     &                           (dTdr(i-1,j,k1)+dTdr(i,j,k2))+         &
     &                           (0.5_r8+SIGN(0.5_r8,-dRdx(i,j,k1)))*   &
     &                           (dTdr(i-1,j,k2)+dTdr(i,j,k1)))
                ad_FX(i,j)=0.0_r8
              END DO
            END DO
          END IF
          IF ((k.eq.0).or.(k.eq.N(ng))) THEN
            DO j=Jstr-1,Jend+1
              DO i=Istr-1,Iend+1
!>              tl_FS(i,j,k2)=0.0_r8
!>
                ad_FS(i,j,k2)=0.0_r8
!>              tl_dTdr(i,j,k2)=0.0_r8
!>
                ad_dTdr(i,j,k2)=0.0_r8
              END DO
            END DO
          ELSE
            DO j=Jstr-1,Jend+1
              DO i=Istr-1,Iend+1
#if defined TS_MIX_MAX_SLOPE
                cff1=SQRT(dRdx(i,j,k2)**2+dRdx(i+1,j,k2)**2+            &
     &                    dRdx(i,j,k1)**2+dRdx(i+1,j,k1)**2+            &
     &                    dRde(i,j,k2)**2+dRde(i,j+1,k2)**2+            &
     &                    dRde(i,j,k1)**2+dRde(i,j+1,k1)**2)
                cff2=0.25_r8*slope_max*                                 &
     &               (z_r(i,j,k+1)-z_r(i,j,k))*cff1
                cff3=MAX(pden(i,j,k)-pden(i,j,k+1),small)
                cff4=MAX(cff2,cff3)
                cff=-1.0_r8/cff4
#elif defined TS_MIX_MIN_STRAT
                cff1=MAX(pden(i,j,k)-pden(i,j,k+1),                     &
     &                   strat_min*(z_r(i,j,k+1)-z_r(i,j,k)))
                cff=-1.0_r8/cff1
#else
                cff1=MAX(pden(i,j,k)-pden(i,j,k+1),eps)
                cff=-1.0_r8/cff1
#endif
!>              tl_FS(i,j,k2)=tl_cff*(z_r(i,j,k+1)-                     &
!>   &                                z_r(i,j,k  ))+                    &
!>   &                        cff*(tl_z_r(i,j,k+1)-                     &
!>   &                             tl_z_r(i,j,k  ))
!>
                adfac=cff*ad_FS(i,j,k2)
                ad_z_r(i,j,k  )=ad_z_r(i,j,k  )-adfac
                ad_z_r(i,j,k+1)=ad_z_r(i,j,k+1)+adfac
                ad_cff=ad_cff+(z_r(i,j,k+1)-                            &
     &                         z_r(i,j,k  ))*ad_FS(i,j,k2)
                ad_FS(i,j,k2)=0.0_r8
!>              tl_dTdr(i,j,k2)=tl_cff*(LapT(i,j,k+1)-                  &
!>   &                                  LapT(i,j,k  ))+                 &
!>   &                          cff*(tl_LapT(i,j,k+1)-                  &
!>   &                               tl_LapT(i,j,k  ))
!>
                adfac=cff*ad_dTdr(i,j,k2)
                ad_LapT(i,j,k  )=ad_LapT(i,j,k  )-adfac
                ad_LapT(i,j,k+1)=ad_LapT(i,j,k+1)+adfac
                ad_cff=ad_cff+(LapT(i,j,k+1)-                           &
     &                         LapT(i,j,k  ))*ad_dTdr(i,j,k2)
                ad_dTdr(i,j,k2)=0.0_r8
#if defined TS_MIX_MAX_SLOPE
!>              tl_cff=cff*cff*tl_cff4
!>
                ad_cff4=ad_cff4+cff*cff*ad_cff
                ad_cff=0.0_r8
!>              tl_cff4=(0.5_r8+SIGN(0.5_r8,cff2-cff3))*tl_cff2+        &
!>   &                  (0.5_r8-SIGN(0.5_r8,cff2-cff3))*tl_cff3
!>
                ad_cff3=ad_cff3+                                        &
     &                  (0.5_r8-SIGN(0.5_r8,cff2-cff3))*ad_cff4
                ad_cff2=ad_cff2+                                        &
     &                  (0.5_r8+SIGN(0.5_r8,cff2-cff3))*ad_cff4
                ad_cff4=0.0_r8
!>              tl_cff3=(0.5_r8+SIGN(0.5_r8,pden(i,j,k)-pden(i,j,k+1)-  &
!>   &                                      small))*                    &
!>   &                  (tl_pden(i,j,k)-tl_pden(i,j,k+1))
!>
                adfac=(0.5_r8+SIGN(0.5_r8,pden(i,j,k)-pden(i,j,k+1)-    &
     &                                    small))*ad_cff3
                ad_pden(i,j,k  )=ad_pden(i,j,k  )+adfac
                ad_pden(i,j,k+1)=ad_pden(i,j,k+1)-adfac
                ad_cff3=0.0_r8
!>              tl_cff2=0.25_r8*slope_max*                              &
!>   &                  ((tl_z_r(i,j,k+1)-tl_z_r(i,j,k))*cff1+          &
!>   &                   (z_r(i,j,k+1)-z_r(i,j,k))*tl_cff1)
!>
                adfac=0.25_r8*slope_max*ad_cff2
                adfac1=adfac*cff1
                ad_cff1=ad_cff1+(z_r(i,j,k+1)-z_r(i,j,k))*adfac
                ad_z_r(i,j,k  )=ad_z_r(i,j,k  )-adfac1
                ad_z_r(i,j,k+1)=ad_z_r(i,j,k+1)+adfac1
                ad_cff2=0.0_r8
                IF (cff1.ne.0.0_r8) THEN
!>                tl_cff1=(dRdx(i  ,j,k2)*tl_dRdx(i  ,j,k2)+            &
!>   &                     dRdx(i+1,j,k2)*tl_dRdx(i+1,j,k2)+            &
!>   &                     dRdx(i  ,j,k1)*tl_dRdx(i  ,j,k1)+            &
!>   &                     dRdx(i+1,j,k1)*tl_dRdx(i+1,j,k1)+            &
!>   &                     dRde(i,j  ,k2)*tl_dRde(i,j  ,k2)+            &
!>   &                     dRde(i,j+1,k2)*tl_dRde(i,j+1,k2)+            &
!>   &                     dRde(i,j  ,k1)*tl_dRde(i,j  ,k1)+            &
!>   &                     dRde(i,j+1,k1)*tl_dRde(i,j+1,k1))/cff1
!>
                  adfac=ad_cff1/cff1
                  ad_dRdx(i  ,j,k1)=ad_dRdx(i  ,j,k1)+                  &
     &                              dRdx(i  ,j,k1)*adfac
                  ad_dRdx(i+1,j,k1)=ad_dRdx(i+1,j,k1)+                  &
     &                              dRdx(i+1,j,k1)*adfac
                  ad_dRdx(i  ,j,k2)=ad_dRdx(i  ,j,k2)+                  &
     &                              dRdx(i  ,j,k2)*adfac
                  ad_dRdx(i+1,j,k2)=ad_dRdx(i+1,j,k2)+                  &
     &                              dRdx(i+1,j,k2)*adfac
                  ad_dRde(i,j  ,k2)=ad_dRde(i,j  ,k2)+                  &
     &                              dRde(i,j  ,k2)*adfac
                  ad_dRde(i,j+1,k2)=ad_dRde(i,j+1,k2)+                  &
     &                              dRde(i,j+1,k2)*adfac
                  ad_dRde(i,j  ,k1)=ad_dRde(i,j  ,k1)+                  &
     &                              dRde(i,j  ,k1)*adfac
                  ad_dRde(i,j+1,k1)=ad_dRde(i,j+1,k1)+                  &
     &                              dRde(i,j+1,k1)*adfac
                  ad_cff1=0.0_r8
                ELSE
!>                tl_cff1=0.0_r8
!>
                  ad_cff1=0.0_r8
                END IF
#elif defined TS_MIX_MIN_STRAT
!>              tl_cff=cff*cff*tl_cff1
!>
                ad_cff1=ad_cff1+cff*cff*ad_cff
                ad_cff=0.0_r8
!>              tl_cff1=(0.5_r8+SIGN(0.5_r8,                            &
!>   &                               pden(i,j,k)-pden(i,j,k+1)-         &
!>   &                               strat_min*(z_r(i,j,k+1)-           &
!>   &                                          z_r(i,j,k  ))))*        &
!>   &                  (tl_pden(i,j,k)-tl_pden(i,j,k+1))+              &
!>   &                  (0.5_r8-SIGN(0.5_r8,                            &
!>   &                               pden(i,j,k)-pden(i,j,k+1)-         &
!>   &                               strat_min*(z_r(i,j,k+1)-           &
!>   &                                          z_r(i,j,k  ))))*        &
!>   &                  (strat_min*(tl_z_r(i,j,k+1)-tl_z_r(i,j,k  )))
!>
                adfac1=(0.5_r8+SIGN(0.5_r8,                             &
     &                              pden(i,j,k)-pden(i,j,k+1)-          &
     &                              strat_min*(z_r(i,j,k+1)-            &
     &                                         z_r(i,j,k  ))))*         &
     &                 ad_cff1
                adfac2=(0.5_r8-SIGN(0.5_r8,                             &
     &                              pden(i,j,k)-pden(i,j,k+1)-          &
     &                              strat_min*(z_r(i,j,k+1)-            &
     &                                         z_r(i,j,k  ))))*         &
     &                 strat_min*ad_cff1
                ad_pden(i,j,k  )=ad_pden(i,j,k  )+adfac1
                ad_pden(i,j,k+1)=ad_pden(i,j,k+1)-adfac1
                ad_z_r(i,j,k  )=ad_z_r(i,j,k  )-adfac2
                ad_z_r(i,j,k+1)=ad_z_r(i,j,k+1)+adfac2
                ad_cff1=0.0_r8
#else
!>              tl_cff=cff*cff*tl_cff1
!>
                ad_cff1=ad_cff1+cff*cff*ad_cff
                ad_cff=0.0_r8
!>              tl_cff1=(0.5_r8+SIGN(0.5_r8,                            &
!>   &                               pden(i,j,k)-pden(i,j,k+1)-eps))*   &
!>   &                  (tl_pden(i,j,k)-tl_pden(i,j,k+1))
!>
                adfac=(0.5_r8+SIGN(0.5_r8,                              &
     &                             pden(i,j,k)-pden(i,j,k+1)-eps))*     &
     &                ad_cff1
                ad_pden(i,j,k  )=ad_pden(i,j,k  )+adfac
                ad_pden(i,j,k+1)=ad_pden(i,j,k+1)-adfac
                ad_cff1=0.0_r8
#endif
              END DO
            END DO
          END IF
          IF (k.lt.N(ng)) THEN
            DO j=Jstr,Jend+1
              DO i=Istr,Iend
                cff=0.5_r8*(pn(i,j)+pn(i,j-1))
#ifdef MASKING
                cff=cff*vmask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                cff=cff*vmask_wet(i,j)
#endif
!>              tl_dTde(i,j,k2)=cff*(tl_LapT(i,j  ,k+1)-                &
!>   &                               tl_LapT(i,j-1,k+1))
!>
                adfac=cff*ad_dTde(i,j,k2)
                ad_LapT(i,j-1,k+1)=ad_LapT(i,j-1,k+1)-adfac
                ad_LapT(i,j  ,k+1)=ad_LapT(i,j  ,k+1)+adfac
                ad_dTde(i,j,k2)=0.0_r8
!>              tl_dRde(i,j,k2)=cff*(tl_pden(i,j  ,k+1)-                &
!>   &                               tl_pden(i,j-1,k+1))
!>
                adfac=cff*ad_dRde(i,j,k2)
                ad_pden(i,j-1,k+1)=ad_pden(i,j-1,k+1)-adfac
                ad_pden(i,j  ,k+1)=ad_pden(i,j  ,k+1)+adfac
                ad_dRde(i,j,k2)=0.0_r8
              END DO
            END DO
            DO j=Jstr,Jend
              DO i=Istr,Iend+1
                cff=0.5_r8*(pm(i,j)+pm(i-1,j))
#ifdef MASKING
                cff=cff*umask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                cff=cff*umask_wet(i,j)
#endif
!>              tl_dTdx(i,j,k2)=cff*(tl_LapT(i  ,j,k+1)-                &
!>   &                               tl_LapT(i-1,j,k+1))
!>
                adfac=cff*ad_dTdx(i,j,k2)
                ad_LapT(i-1,j,k+1)=ad_LapT(i-1,j,k+1)-adfac
                ad_LapT(i  ,j,k+1)=ad_LapT(i  ,j,k+1)+adfac
                ad_dTdx(i,j,k2)=0.0_r8
!>              tl_dRdx(i,j,k2)=cff*(tl_pden(i  ,j,k+1)-                &
!>   &                               tl_pden(i-1,j,k+1))
!>
                adfac=cff*ad_dRdx(i,j,k2)
                ad_pden(i-1,j,k+1)=ad_pden(i-1,j,k+1)-adfac
                ad_pden(i  ,j,k+1)=ad_pden(i  ,j,k+1)+adfac
                ad_dRdx(i,j,k2)=0.0_r8
              END DO
            END DO
          END IF
!
!  Compute new storage recursive indices.
!
          kt=k2
          k2=k1
          k1=kt
        END DO K_LOOP2
!
!  Apply adjoint boundary conditions (except periodic; closed or
!  gradient) to the first harmonic operator.
!
        IF (.not.(CompositeGrid(inorth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(ieast ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%NorthEast_Corner(tile)) THEN
            DO k=1,N(ng)
!>            tl_LapT(Iend+1,Jend+1,k)=0.5_r8*                          &
!>   &                                 (tl_LapT(Iend  ,Jend+1,k)+       &
!>   &                                  tl_LapT(Iend+1,Jend  ,k))
!>
              adfac=0.5_r8*ad_LapT(Iend+1,Jend+1,k)
              ad_LapT(Iend+1,Jend  ,k)=ad_LapT(Iend+1,Jend  ,k)+adfac
              ad_LapT(Iend  ,Jend+1,k)=ad_LapT(Iend  ,Jend+1,k)+adfac
              ad_LapT(Iend+1,Jend+1,k)=0.0_r8
            END DO
          END IF
        END IF

        IF (.not.(CompositeGrid(inorth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(iwest ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%NorthWest_Corner(tile)) THEN
            DO k=1,N(ng)
!>            tl_LapT(Istr-1,Jend+1,k)=0.5_r8*                          &
!>   &                                 (tl_LapT(Istr  ,Jend+1,k)+       &
!>   &                                  tl_LapT(Istr-1,Jend  ,k))
!>
              adfac=0.5_r8*ad_LapT(Istr-1,Jend+1,k)
              ad_LapT(Istr-1,Jend  ,k)=ad_LapT(Istr-1,Jend  ,k)+adfac
              ad_LapT(Istr  ,Jend+1,k)=ad_LapT(Istr  ,Jend+1,k)+adfac
              ad_LapT(Istr-1,Jend+1,k)=0.0_r8
            END DO
          END IF
        END IF

        IF (.not.(CompositeGrid(isouth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(ieast ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%SouthEast_Corner(tile)) THEN
            DO k=1,N(ng)
!>            tl_LapT(Iend+1,Jstr-1,k)=0.5_r8*                          &
!>   &                                 (tl_LapT(Iend  ,Jstr-1,k)+       &
!>   &                                  tl_LapT(Iend+1,Jstr  ,k))
!>
              adfac=0.5_r8*ad_LapT(Iend+1,Jstr-1,k)
              ad_LapT(Iend  ,Jstr-1,k)=ad_LapT(Iend  ,Jstr-1,k)+adfac
              ad_LapT(Iend+1,Jstr  ,k)=ad_LapT(Iend+1,Jstr  ,k)+adfac
              ad_LapT(Iend+1,Jstr-1,k)=0.0_r8
            END DO
          END IF
        END IF

        IF (.not.(CompositeGrid(isouth,ng).or.NSperiodic(ng).or.        &
     &            CompositeGrid(iwest ,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%SouthWest_Corner(tile)) THEN
            DO k=1,N(ng)
!>            tl_LapT(Istr-1,Jstr-1,k)=0.5_r8*                          &
!>   &                                 (tl_LapT(Istr  ,Jstr-1,k)+       &
!>                                      tl_LapT(Istr-1,Jstr  ,k))
!>
              adfac=0.5_r8*ad_LapT(Istr-1,Jstr-1,k)
              ad_LapT(Istr  ,Jstr-1,k)=ad_LapT(Istr  ,Jstr-1,k)+adfac
              ad_LapT(Istr-1,Jstr  ,k)=ad_LapT(Istr-1,Jstr  ,k)+adfac
              ad_LapT(Istr-1,Jstr-1,k)=0.0_r8
            END DO
          END IF
        END IF
!
        IF (.not.(CompositeGrid(inorth,ng).or.NSperiodic(ng))) THEN
          IF (DOMAIN(ng)%Northern_Edge(tile)) THEN
            IF (ad_LBC(inorth,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO i=Imin,Imax
!>                tl_LapT(i,Jend+1,k)=0.0_r8
!>
                  ad_LapT(i,Jend+1,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO i=Imin,Imax
!>                tl_LapT(i,Jend+1,k)=tl_LapT(i,Jend,k)
!>
                  ad_LapT(i,Jend,k)=ad_LapT(i,Jend,k)+                  &
     &                              ad_LapT(i,Jend+1,k)
                  ad_LapT(i,Jend+1,k)=0.0_r8
                END DO
              END DO
            END IF
          END IF
        END IF
!
        IF (.not.(CompositeGrid(isouth,ng).or.NSperiodic(ng))) THEN
          IF (DOMAIN(ng)%Southern_Edge(tile)) THEN
            IF (ad_LBC(isouth,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO i=Imin,Imax
!>                tl_LapT(i,Jstr-1,k)=0.0_r8
!>
                  ad_LapT(i,Jstr-1,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO i=Imin,Imax
!>                tl_LapT(i,Jstr-1,k)=tl_LapT(i,Jstr,k)
!>
                  ad_LapT(i,Jstr,k)=ad_LapT(i,Jstr,k)+                  &
     &                              ad_LapT(i,Jstr-1,k)
                  ad_LapT(i,Jstr-1,k)=0.0_r8
                END DO
              END DO
            END IF
          END IF
        END IF
!
        IF (.not.(CompositeGrid(ieast,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%Eastern_Edge(tile)) THEN
            IF (ad_LBC(ieast,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO j=Jmin,Jmax
!>                tl_LapT(Iend+1,j,k)=0.0_r8
!>
                  ad_LapT(Iend+1,j,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO j=Jmin,Jmax
!>                tl_LapT(Iend+1,j,k)=tl_LapT(Iend,j,k)
!>
                  ad_LapT(Iend,j,k)=ad_LapT(Iend,j,k)+                  &
     &                              ad_LapT(Iend+1,j,k)
                  ad_LapT(Iend+1,j,k)=0.0_r8
                END DO
              END DO
            END IF
          END IF
        END IF
!
        IF (.not.(CompositeGrid(iwest,ng).or.EWperiodic(ng))) THEN
          IF (DOMAIN(ng)%Western_Edge(tile)) THEN
            IF (ad_LBC(iwest,isTvar(itrc),ng)%closed) THEN
              DO k=1,N(ng)
                DO j=Jmin,Jmax
!>                tl_LapT(Istr-1,j,k)=0.0_r8
!>
                  ad_LapT(Istr-1,j,k)=0.0_r8
                END DO
              END DO
            ELSE
              DO k=1,N(ng)
                DO j=Jmin,Jmax
!>                tl_LapT(Istr-1,j,k)=tl_LapT(Istr,j,k)
!>
                  ad_LapT(Istr,j,k)=ad_LapT(Istr,j,k)+                  &
     &                              ad_LapT(Istr-1,j,k)
                  ad_LapT(Istr-1,j,k)=0.0_r8
                END DO
              END DO
            END IF
          END IF
        END IF
!
!-----------------------------------------------------------------------
!  Compute first adjoint harmonic operator, without mixing coefficient.
!  Multiply by the metrics of the second harmonic operator.
!-----------------------------------------------------------------------
!
!  Compute adjoint of starting recursive indices k1 and k2.
!
        k1=2
        k2=1
        DO k=0,N(ng)
!!
!!  Note: The following code is equivalent to
!!
!!        kt=k1
!!        k1=k2
!!        k2=kt
!!
!!  We use the adjoint of above code.
!!
          k1=k2
          k2=3-k1
        END DO
!
!  Compute required basic state fields. Need to look forward in "kk"
!  index.
!
        K_LOOP3: DO k=N(ng),0,-1
          k2b=1
          DO kk=0,k
            k1b=k2b
            k2b=3-k1b
            IF (kk.lt.N(ng)) THEN
              DO j=Jmin,Jmax
                DO i=Imin,Imax+1
                  cff=0.5_r8*(pm(i,j)+pm(i-1,j))
#ifdef MASKING
                  cff=cff*umask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                  cff=cff*umask_wet(i,j)
#endif
                  dRdx(i,j,k2b)=cff*(pden(i  ,j,kk+1)-                  &
     &                               pden(i-1,j,kk+1))
#if defined TS_MIX_STABILITY
                  dTdx(i,j,k2b)=cff*(0.75_r8*(t(i  ,j,kk+1,nrhs,itrc)-  &
     &                                        t(i-1,j,kk+1,nrhs,itrc))+ &
     &                               0.25_r8*(t(i  ,j,kk+1,nstp,itrc)-  &
     &                                        t(i-1,j,kk+1,nstp,itrc)))
#elif defined TS_MIX_CLIMA
                  IF (LtracerCLM(itrc,ng)) THEN
                    dTdx(i,j,k2b)=cff*((t(i  ,j,kk+1,nrhs,itrc)-        &
     &                                  tclm(i  ,j,kk+1,itrc))-         &
     &                                 (t(i-1,j,kk+1,nrhs,itrc)-        &
     &                                  tclm(i-1,j,kk+1,itrc)))
                  ELSE
                    dTdx(i,j,k2b)=cff*(t(i  ,j,kk+1,nrhs,itrc)-         &
     &                                 t(i-1,j,kk+1,nrhs,itrc))
                  END IF
#else
                  dTdx(i,j,k2b)=cff*(t(i  ,j,kk+1,nrhs,itrc)-           &
     &                               t(i-1,j,kk+1,nrhs,itrc))
#endif
                END DO
              END DO
              IF (kk.eq.0) THEN
                DO j=Jmin,Jmax
                  DO i=Imin,Imax+1
                    dRdx(i,j,k1b)=0.0_r8
                    dTdx(i,j,k1b)=0.0_r8
                  END DO
                END DO
              END IF
              DO j=Jmin,Jmax+1
                DO i=Imin,Imax
                  cff=0.5_r8*(pn(i,j)+pn(i,j-1))
#ifdef MASKING
                  cff=cff*vmask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                  cff=cff*vmask_wet(i,j)
#endif
                  dRde(i,j,k2b)=cff*(pden(i,j  ,kk+1)-                  &
     &                               pden(i,j-1,kk+1))
#if defined TS_MIX_STABILITY
                  dTde(i,j,k2b)=cff*(0.75_r8*(t(i,j  ,kk+1,nrhs,itrc)-  &
     &                                        t(i,j-1,kk+1,nrhs,itrc))+ &
     &                               0.25_r8*(t(i,j  ,kk+1,nstp,itrc)-  &
     &                                        t(i,j-1,kk+1,nstp,itrc)))
#elif defined TS_MIX_CLIMA
                  IF (LtracerCLM(itrc,ng)) THEN
                    dTde(i,j,k2b)=cff*((t(i,j  ,kk+1,nrhs,itrc)-        &
     &                                  tclm(i,j  ,kk+1,itrc))-         &
     &                                 (t(i,j-1,kk+1,nrhs,itrc)-        &
     &                                  tclm(i,j-1,kk+1,itrc)))
                  ELSE
                    dTde(i,j,k2b)=cff*(t(i,j  ,kk+1,nrhs,itrc)-         &
     &                                 t(i,j-1,kk+1,nrhs,itrc))
                  END IF
#else
                  dTde(i,j,k2b)=cff*(t(i,j  ,kk+1,nrhs,itrc)-           &
     &                               t(i,j-1,kk+1,nrhs,itrc))
#endif
                END DO
              END DO
              IF (kk.eq.0) THEN
                DO j=Jmin,Jmax+1
                  DO i=Imin,Imax
                    dRde(i,j,k1b)=0.0_r8
                    dTde(i,j,k1b)=0.0_r8
                  END DO
                END DO
              END IF
            END IF
            IF ((kk.eq.0).or.(kk.eq.N(ng))) THEN
              DO j=-1+Jmin,Jmax+1
                DO i=-1+Imin,Imax+1
                  dTdr(i,j,k2b)=0.0_r8
                  FS(i,j,k2b)=0.0_r8
                END DO
              END DO
              IF (kk.eq.0) THEN
                DO j=-1+Jmin,Jmax+1
                  DO i=-1+Imin,Imax+1
                    dTdr(i,j,k1b)=0.0_r8
                    FS(i,j,k1b)=0.0_r8
                  END DO
                END DO
              END IF
            ELSE
              DO j=-1+Jmin,Jmax+1
                DO i=-1+Imin,Imax+1
#if defined TS_MIX_MAX_SLOPE
                  cff1=SQRT(dRdx(i,j,k2b)**2+dRdx(i+1,j,k2b)**2+        &
     &                      dRdx(i,j,k1b)**2+dRdx(i+1,j,k1b)**2+        &
     &                      dRde(i,j,k2b)**2+dRde(i,j+1,k2b)**2+        &
     &                      dRde(i,j,k1b)**2+dRde(i,j+1,k1b)**2)
                  cff2=0.25_r8*slope_max*                               &
     &                 (z_r(i,j,kk+1)-z_r(i,j,kk))*cff1
                  cff3=MAX(pden(i,j,kk)-pden(i,j,kk+1),small)
                  cff4=MAX(cff2,cff3)
                  cff=-1.0_r8/cff4
#elif defined TS_MIX_MIN_STRAT
                  cff1=MAX(pden(i,j,kk)-pden(i,j,kk+1),                 &
     &                     strat_min*(z_r(i,j,kk+1)-z_r(i,j,kk)))
                  cff=-1.0_r8/cff1
#else
                  cff1=MAX(pden(i,j,kk)-pden(i,j,kk+1),eps)
                  cff=-1.0_r8/cff1
#endif
#if defined TS_MIX_STABILITY
                  dTdr(i,j,k2b)=cff*(0.75_r8*(t(i,j,kk+1,nrhs,itrc)-    &
     &                                        t(i,j,kk  ,nrhs,itrc))+   &
     &                               0.25_r8*(t(i,j,kk+1,nstp,itrc)-    &
     &                                        t(i,j,kk  ,nstp,itrc)))
#elif defined TS_MIX_CLIMA
                  IF (LtracerCLM(itrc,ng)) THEN
                    dTdr(i,j,k2b)=cff*((t(i,j,kk+1,nrhs,itrc)-          &
       &                                tclm(i,j,kk+1,itrc))-           &
     &                                 (t(i,j,kk  ,nrhs,itrc)-          &
     &                                  tclm(i,j,kk  ,itrc)))
                  ELSE
                    dTdr(i,j,k2b)=cff*(t(i,j,kk+1,nrhs,itrc)-           &
     &                                 t(i,j,kk  ,nrhs,itrc))
                  END IF
#else
                  dTdr(i,j,k2b)=cff*(t(i,j,kk+1,nrhs,itrc)-             &
     &                               t(i,j,kk  ,nrhs,itrc))
#endif
                  FS(i,j,k2b)=cff*(z_r(i,j,kk+1)-                       &
     &                             z_r(i,j,kk  ))
                END DO
              END DO
            END IF
            IF (kk.gt.0) THEN
              DO j=Jmin,Jmax
                DO i=Imin,Imax+1
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                  cff=0.5_r8*diff3d_u(i,j,k)*on_u(i,j)
# else
                  cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i-1,j,k))*      &
     &                on_u(i,j)
# endif
#else
                  cff=0.25_r8*(diff4(i,j,itrc)+diff4(i-1,j,itrc))*      &
     &                on_u(i,j)
#endif
                  FX(i,j)=cff*                                          &
     &                    (Hz(i,j,kk)+Hz(i-1,j,kk))*                    &
     &                    (dTdx(i  ,j,k1b)-                             &
     &                     0.5_r8*(MAX(dRdx(i,j,k1b),0.0_r8)*           &
     &                                (dTdr(i-1,j,k1b)+                 &
     &                                 dTdr(i  ,j,k2b))+                &
     &                             MIN(dRdx(i,j,k1b),0.0_r8)*           &
     &                                (dTdr(i-1,j,k2b)+                 &
     &                                 dTdr(i  ,j,k1b))))
                END DO
              END DO
              DO j=Jmin,Jmax+1
                DO i=Imin,Imax
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                  cff=0.5_r8*diff3d_v(i,j,k)*om_v(i,j)
# else
                  cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i,j-1,k))*      &
     &                om_v(i,j)
# endif
#else
                  cff=0.25_r8*(diff4(i,j,itrc)+diff4(i,j-1,itrc))*      &
     &                om_v(i,j)
#endif
                  FE(i,j)=cff*                                          &
     &                    (Hz(i,j,kk)+Hz(i,j-1,kk))*                    &
     &                    (dTde(i,j,k1b)-                               &
     &                     0.5_r8*(MAX(dRde(i,j,k1b),0.0_r8)*           &
     &                                (dTdr(i,j-1,k1b)+                 &
     &                                 dTdr(i,j  ,k2b))+                &
     &                             MIN(dRde(i,j,k1b),0.0_r8)*           &
     &                                (dTdr(i,j-1,k2b)+                 &
     &                                 dTdr(i,j  ,k1b))))
                END DO
              END DO
              IF (kk.lt.N(ng)) THEN
                DO j=Jmin,Jmax
                  DO i=Imin,Imax
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                    difx=0.125_r8*                                      &
     &                   (diff3d_u(i,j,kk  )+diff3d_u(i+1,j,kk  )+      &
     &                    diff3d_u(i,j,kk+1)+diff3d_u(i+1,j,kk+1))
                    dife=0.125_r8*                                      &
     &                   (diff3d_v(i,j,kk  )+diff3d_v(i,j+1,kk  )+      &
     &                    diff3d_v(i,j,kk+1)+diff3d_v(i,j+1,kk+1))
# else
                    difx=0.5_r8*diff3d_r(i,j,kk)
                    dife=difx
# endif
#else
                    difx=0.5_r8*diff4(i,j,itrc)
                    dife=difx
#endif
                    cff1=MAX(dRdx(i  ,j,k1b),0.0_r8)
                    cff2=MAX(dRdx(i+1,j,k2b),0.0_r8)
                    cff3=MIN(dRdx(i  ,j,k2b),0.0_r8)
                    cff4=MIN(dRdx(i+1,j,k1b),0.0_r8)
                    cff=difx*                                           &
     &                  (cff1*(cff1*dTdr(i,j,k2b)-dTdx(i  ,j,k1b))+     &
     &                   cff2*(cff2*dTdr(i,j,k2b)-dTdx(i+1,j,k2b))+     &
     &                   cff3*(cff3*dTdr(i,j,k2b)-dTdx(i  ,j,k2b))+     &
     &                   cff4*(cff4*dTdr(i,j,k2b)-dTdx(i+1,j,k1b)))
                    cff1=MAX(dRde(i,j  ,k1b),0.0_r8)
                    cff2=MAX(dRde(i,j+1,k2b),0.0_r8)
                    cff3=MIN(dRde(i,j  ,k2b),0.0_r8)
                    cff4=MIN(dRde(i,j+1,k1b),0.0_r8)
                    cff=cff+                                            &
     &                  dife*                                           &
     &                  (cff1*(cff1*dTdr(i,j,k2b)-dTde(i,j  ,k1b))+     &
     &                   cff2*(cff2*dTdr(i,j,k2b)-dTde(i,j+1,k2b))+     &
     &                   cff3*(cff3*dTdr(i,j,k2b)-dTde(i,j  ,k2b))+     &
     &                   cff4*(cff4*dTdr(i,j,k2b)-dTde(i,j+1,k1b)))
                    FS1(i,j,k2b)=FS(i,j,k2b)                 ! recursive
                    FS (i,j,k2b)=cff*FS(i,j,k2b)
                  END DO
                END DO
                IF (kk.eq.0) THEN
                  DO j=Jmin,Jmax
                    DO i=Imin,Imax
                      FS1(i,j,k1b)=0.0_r8
                      FS (i,j,k1b)=0.0_r8
                    END DO
                  END DO
                END IF
              END IF
            END IF
          END DO
!
!  Compute adjoint first harmonic operator, without mixing coefficient.
!  Multiply by the metrics of the second harmonic operator.  Save
!  into work array "LapT".
!
          IF (k.gt.0) THEN
            DO j=Jmin,Jmax
              DO i=Imin,Imax
                cff=pm(i,j)*pn(i,j)
                cff1=1.0_r8/Hz(i,j,k)
!>              tl_LapT(i,j,k)=tl_cff1*(cff*                            &
!>   &                                  (FX(i+1,j)-FX(i,j)+             &
!>   &                                   FE(i,j+1)-FE(i,j))+            &
!>   &                                  (FS(i,j,k2)-FS(i,j,k1)))+       &
!>   &                         cff1*(cff*                               &
!>   &                               (tl_FX(i+1,j)-tl_FX(i,j)+          &
!>   &                                tl_FE(i,j+1)-tl_FE(i,j))+         &
!>   &                               (tl_FS(i,j,k2)-tl_FS(i,j,k1)))
!>
                adfac=cff1*ad_LapT(i,j,k)
                adfac1=adfac*cff
                ad_FS(i,j,k1)=ad_FS(i,j,k1)-adfac
                ad_FS(i,j,k2)=ad_FS(i,j,k2)+adfac
                ad_FE(i,j  )=ad_FE(i,j  )-adfac1
                ad_FE(i,j+1)=ad_FE(i,j+1)+adfac1
                ad_FX(i  ,j)=ad_FX(i  ,j)-adfac1
                ad_FX(i+1,j)=ad_FX(i+1,j)+adfac1
                ad_cff1=ad_cff1+(cff*                                   &
     &                           (FX(i+1,j)-FX(i,j)+                    &
     &                            FE(i,j+1)-FE(i,j))+                   &
     &                           (FS(i,j,k2)-FS(i,j,k1)))*              &
     &                           ad_LapT(i,j,k)
                ad_LapT(i,j,k)=0.0_r8
!>              tl_cff1=-cff1*cff1*tl_Hz(i,j,k)
!>
                ad_Hz(i,j,k)=ad_Hz(i,j,k)-cff1*cff1*ad_cff1
                ad_cff1=0.0_r8
              END DO
            END DO
            IF (k.lt.N(ng)) THEN
              DO j=Jmin,Jmax
                DO i=Imin,Imax
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                  difx=0.125_r8*(diff3d_u(i,j,k  )+diff3d_u(i+1,j,k  )+ &
     &                           diff3d_u(i,j,k+1)+diff3d_u(i+1,j,k+1))
                  dife=0.125_r8*(diff3d_v(i,j,k  )+diff3d_v(i,j+1,k  )+ &
     &                           diff3d_v(i,j,k+1)+diff3d_v(i,j+1,k+1))
# else
                  difx=0.5_r8*diff3d_r(i,j,k)
                  dife=difx
# endif
#else
                  difx=0.5_r8*diff4(i,j,itrc)
                  dife=difx
#endif
                  cff1=MAX(dRdx(i  ,j,k1),0.0_r8)
                  cff2=MAX(dRdx(i+1,j,k2),0.0_r8)
                  cff3=MIN(dRdx(i  ,j,k2),0.0_r8)
                  cff4=MIN(dRdx(i+1,j,k1),0.0_r8)
                  cff=difx*                                             &
     &                (cff1*(cff1*dTdr(i,j,k2)-dTdx(i  ,j,k1))+         &
     &                 cff2*(cff2*dTdr(i,j,k2)-dTdx(i+1,j,k2))+         &
     &                 cff3*(cff3*dTdr(i,j,k2)-dTdx(i  ,j,k2))+         &
     &                 cff4*(cff4*dTdr(i,j,k2)-dTdx(i+1,j,k1)))
                  cff1=MAX(dRde(i,j  ,k1),0.0_r8)
                  cff2=MAX(dRde(i,j+1,k2),0.0_r8)
                  cff3=MIN(dRde(i,j  ,k2),0.0_r8)
                  cff4=MIN(dRde(i,j+1,k1),0.0_r8)
                  cff=cff+                                              &
     &                dife*                                             &
     &                (cff1*(cff1*dTdr(i,j,k2)-dTde(i,j  ,k1))+         &
     &                 cff2*(cff2*dTdr(i,j,k2)-dTde(i,j+1,k2))+         &
     &                 cff3*(cff3*dTdr(i,j,k2)-dTde(i,j  ,k2))+         &
     &                 cff4*(cff4*dTdr(i,j,k2)-dTde(i,j+1,k1)))
!>                tl_FS(i,j,k2)=tl_cff*FS(i,j,k2)+                      &
!>   &                          cff*tl_FS(i,j,k2)            ! recursive
!>                                                           ! use FS1
!>
                  ad_FS(i,j,k2)=cff*ad_FS(i,j,k2)
                  ad_cff=ad_cff+FS1(i,j,k2)*ad_FS(i,j,k2)
!>                tl_cff=tl_cff+                                        &
!>   &                   dife*                                          &
!>   &                   (tl_cff1*(cff1*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j  ,k1))+                     &
!>   &                    tl_cff2*(cff2*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j+1,k2))+                     &
!>   &                    tl_cff3*(cff3*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j  ,k2))+                     &
!>   &                    tl_cff4*(cff4*dTdr(i,j,k2)-                   &
!>   &                             dTde(i,j+1,k1))+                     &
!>   &                    cff1*(tl_cff1*dTdr(i,j,k2)+                   &
!>   &                          cff1*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j  ,k1))+                     &
!>   &                    cff2*(tl_cff2*dTdr(i,j,k2)+                   &
!>   &                          cff2*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j+1,k2))+                     &
!>   &                    cff3*(tl_cff3*dTdr(i,j,k2)+                   &
!>   &                          cff3*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j  ,k2))+                     &
!>   &                    cff4*(tl_cff4*dTdr(i,j,k2)+                   &
!>   &                          cff4*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTde(i,j+1,k1)))
!>
                  adfac*dife*ad_cff
                  ad_cff1=ad_cff1+                                      &
     &                    (2.0_r8*cff1*dTdr(i,j,k2)-dTde(i,j  ,k1))*    &
     &                    adfac
                  ad_cff2=ad_cff2+                                      &
     &                    (2.0_r8*cff2*dTdr(i,j,k2)-dTde(i,j+1,k2))*    &
     &                    adfac
                  ad_cff3=ad_cff3+                                      &
     &                    (2.0_r8*cff3*dTdr(i,j,k2)-dTde(i,j  ,k2))*    &
     &                    adfac
                  ad_cff4=ad_cff4+                                      &
     &                    (2.0_r8*cff4*dTdr(i,j,k2)-dTde(i,j+1,k1))*    &
     &                    adfac
                  ad_dTdr(i,j,k2)=ad_dTdr(i,j,k2)+                      &
     &                            (cff1*cff1+                           &
     &                             cff2*cff2+                           &
     &                             cff3*cff3+                           &
     &                             cff4*cff4)*adfac
                  ad_dTde(i,j  ,k1)=ad_dTde(i,j  ,k1)-cff1*adfac
                  ad_dTde(i,j+1,k2)=ad_dTde(i,j+1,k2)-cff2*adfac
                  ad_dTde(i,j  ,k2)=ad_dTde(i,j  ,k2)-cff3*adfac
                  ad_dTde(i,j+1,k1)=ad_dTde(i,j+1,k1)-cff4*adfac
!>                tl_cff4=(0.5_r8+SIGN(0.5_r8,-dRde(i,j+1,k1)))*        &
!>   &                    tl_dRde(i,j+1,k1)
!>
                  ad_dRde(i,j+1,k1)=ad_dRde(i,j+1,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRde(i,j+1,k1)))*     &
     &                              ad_cff4
                  ad_cff4=0.0_r8
!>                tl_cff3=(0.5_r8+SIGN(0.5_r8,-dRde(i,j  ,k2)))*        &
!>   &                    tl_dRde(i,j  ,k2)
!>
                  ad_dRde(i,j  ,k2)=ad_dRde(i,j  ,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRde(i,j  ,k2)))*     &
     &                              ad_cff3
                  ad_cff3=0.0_r8
!>                tl_cff2=(0.5_r8+SIGN(0.5_r8, dRde(i,j+1,k2)))*        &
!>   &                    tl_dRde(i,j+1,k2)
!>
                  ad_dRde(i,j+1,k2)=ad_dRde(i,j+1,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRde(i,j+1,k2)))*     &
     &                              ad_cff2
                  ad_cff2=0.0_r8
!>                tl_cff1=(0.5_r8+SIGN(0.5_r8, dRde(i,j  ,k1)))*        &
!>   &                    tl_dRde(i,j  ,k1)
!>
                  ad_dRde(i  ,j,k1)=ad_dRde(i  ,j,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRde(i  ,j,k1)))*     &
     &                              ad_cff1
                  ad_cff1=0.0_r8
                  cff1=MAX(dRdx(i  ,j,k1),0.0_r8)
                  cff2=MAX(dRdx(i+1,j,k2),0.0_r8)
                  cff3=MIN(dRdx(i  ,j,k2),0.0_r8)
                  cff4=MIN(dRdx(i+1,j,k1),0.0_r8)
!>                tl_cff=difx*                                          &
!>   &                   (tl_cff1*(cff1*dTdr(i  ,j,k2)-                 &
!>   &                             dTdx(i  ,j,k1))+                     &
!>   &                    tl_cff2*(cff2*dTdr(i,j,k2)-                   &
!>   &                             dTdx(i+1,j,k2))+                     &
!>   &                    tl_cff3*(cff3*dTdr(i,j,k2)-                   &
!>   &                             dTdx(i  ,j,k2))+                     &
!>   &                    tl_cff4*(cff4*dTdr(i,j,k2)-                   &
!>   &                             dTdx(i+1,j,k1))+                     &
!>   &                    cff1*(tl_cff1*dTdr(i,j,k2)+                   &
!>   &                          cff1*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i  ,j,k1))+                     &
!>   &                    cff2*(tl_cff2*dTdr(i,j,k2)+                   &
!>   &                          cff2*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i+1,j,k2))+                     &
!>   &                    cff3*(tl_cff3*dTdr(i,j,k2)+                   &
!>   &                          cff3*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i  ,j,k2))+                     &
!>   &                    cff4*(tl_cff4*dTdr(i,j,k2)+                   &
!>   &                          cff4*tl_dTdr(i,j,k2)-                   &
!>   &                          tl_dTdx(i+1,j,k1)))
!>
                  adfac=difx*ad_cff
                  ad_cff1=ad_cff1+                                      &
     &                    (2.0_r8*cff1*dTdr(i,j,k2)-dTdx(i  ,j,k1))*    &
     &                    adfac
                  ad_cff2=ad_cff2+                                      &
     &                    (2.0_r8*cff2*dTdr(i,j,k2)-dTdx(i+1,j,k2))*    &
     &                    adfac
                  ad_cff3=ad_cff3+                                      &
     &                    (2.0_r8*cff3*dTdr(i,j,k2)-dTdx(i  ,j,k2))*    &
     &                    adfac
                  ad_cff4=ad_cff4+                                      &
     &                    (2.0_r8*cff4*dTdr(i,j,k2)-dTdx(i+1,j,k1))*    &
     &                    adfac
                  ad_dTdr(i,j,k2)=ad_dTdr(i,j,k2)+                      &
     &                            (cff1*cff1+                           &
     &                             cff2*cff2+                           &
     &                             cff3*cff3+                           &
     &                             cff4*cff4)*adfac
                  ad_dTdx(i  ,j,k1)=ad_dTdx(i  ,j,k1)-cff1*adfac
                  ad_dTdx(i+1,j,k2)=ad_dTdx(i+1,j,k2)-cff2*adfac
                  ad_dTdx(i  ,j,k2)=ad_dTdx(i  ,j,k2)-cff3*adfac
                  ad_dTdx(i+1,j,k1)=ad_dTdx(i+1,j,k1)-cff4*adfac
                  ad_cff=0.0_r8
!>                tl_cff4=(0.5_r8+SIGN(0.5_r8,-dRdx(i+1,j,k1)))*        &
!>   &                    tl_dRdx(i+1,j,k1)
!>
                  ad_dRdx(i+1,j,k1)=ad_dRdx(i+1,j,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRdx(i+1,j,k1)))*     &
     &                              ad_cff4
                  ad_cff4=0.0_r8
!>                tl_cff3=(0.5_r8+SIGN(0.5_r8,-dRdx(i  ,j,k2)))*        &
!>   &                    tl_dRdx(i  ,j,k2)
!>
                  ad_dRdx(i  ,j,k2)=ad_dRdx(i  ,j,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                           -dRdx(i  ,j,k2)))*     &
     &                              ad_cff3
                  ad_cff3=0.0_r8
!>                tl_cff2=(0.5_r8+SIGN(0.5_r8, dRdx(i+1,j,k2)))*        &
!>   &                    tl_dRdx(i+1,j,k2)
!>
                  ad_dRdx(i+1,j,k2)=ad_dRdx(i+1,j,k2)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRdx(i+1,j,k2)))*     &
     &                              ad_cff2
                  ad_cff2=0.0_r8
!>                tl_cff1=(0.5_r8+SIGN(0.5_r8, dRdx(i  ,j,k1)))*        &
!>   &                    tl_dRdx(i  ,j,k1)
!>
                  ad_dRdx(i  ,j,k1)=ad_dRdx(i  ,j,k1)+                  &
     &                              (0.5_r8+SIGN(0.5_r8,                &
     &                                            dRdx(i  ,j,k1)))*     &
     &                              ad_cff1
                  ad_cff1=0.0_r8
                END DO
              END DO
            END IF
            DO j=Jmin,Jmax+1
              DO i=Imin,Imax
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                cff=0.5_r8*diff3d_v(i,j,k)*om_v(i,j)
# else
                cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i,j-1,k))*        &
     &              om_v(i,j)
# endif
#else
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i,j-1,itrc))*        &
     &              om_v(i,j)
#endif
!>              tl_FE(i,j)=cff*                                         &
!>   &                     ((tl_Hz(i,j,k)+tl_Hz(i,j-1,k))*              &
!>   &                      (dTde(i,j,k1)-                              &
!>   &                       0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i,j-1,k1)+                &
!>   &                                   dTdr(i,j  ,k2))+               &
!>   &                               MIN(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i,j-1,k2)+                &
!>   &                                   dTdr(i,j  ,k1))))+             &
!>   &                      (Hz(i,j,k)+Hz(i,j-1,k))*                    &
!>   &                      (tl_dTde(i,j,k1)-                           &
!>   &                       0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i,j-1,k1)+             &
!>   &                                   tl_dTdr(i,j  ,k2))+            &
!>   &                               MIN(dRde(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i,j-1,k2)+             &
!>   &                                   tl_dTdr(i,j  ,k1)))-           &
!>   &                       0.5_r8*((0.5_r8+                           &
!>   &                                SIGN(0.5_r8, dRde(i,j,k1)))*      &
!>   &                               tl_dRde(i,j,k1)*                   &
!>   &                               (dTdr(i,j-1,k1)+dTdr(i,j,k2))+     &
!>   &                               (0.5_r8+                           &
!>   &                                SIGN(0.5_r8,-dRde(i,j,k1)))*      &
!>   &                               tl_dRde(i,j,k1)*                   &
!>   &                               (dTdr(i,j-1,k2)+dTdr(i,j,k1)))))
!>
                adfac=cff*ad_FE(i,j)
                adfac1=adfac*(dTde(i,j,k1)-                             &
     &                        0.5_r8*(MAX(dRde(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i,j-1,k1)+               &
     &                                    dTdr(i  ,j,k2))+              &
     &                                MIN(dRde(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i,j-1,k2)+               &
     &                                    dTdr(i  ,j,k1))))
                adfac2=adfac*(Hz(i,j,k)+Hz(i,j-1,k))
                adfac3=adfac2*0.5_r8*MAX(dRde(i,j,k1),0.0_r8)
                adfac4=adfac2*0.5_r8*MIN(dRde(i,j,k1),0.0_r8)
                ad_Hz(i,j-1,k)=ad_Hz(i,j-1,k)+adfac1
                ad_Hz(i,j  ,k)=ad_Hz(i,j  ,k)+adfac1
                ad_dTde(i,j,k1)=ad_dTde(i,j,k1)+adfac2
                ad_dTdr(i,j-1,k1)=ad_dTdr(i,j-1,k1)-adfac3
                ad_dTdr(i,j,  k2)=ad_dTdr(i,j  ,k2)-adfac3
                ad_dTdr(i,j-1,k2)=ad_dTdr(i,j-1,k2)-adfac4
                ad_dTdr(i,j  ,k1)=ad_dTdr(i,j  ,k1)-adfac4
                ad_dRde(i,j,k1)=ad_dRde(i,j,k1)-                        &
     &                          adfac2*0.5_r8*                          &
     &                          ((0.5_r8+SIGN(0.5_r8, dRde(i,j,k1)))*   &
     &                           (dTdr(i,j-1,k1)+dTdr(i,j,k2))+         &
     &                           (0.5_r8+SIGN(0.5_r8,-dRde(i,j,k1)))*   &
     &                           (dTdr(i,j-1,k2)+dTdr(i,j,k1)))
                ad_FE(i,j)=0.0_r8
              END DO
            END DO
            DO j=Jmin,Jmax
              DO i=Imin,Imax+1
#ifdef DIFF_3DCOEF
# ifdef TS_U3ADV_SPLIT
                cff=0.5_r8*diff3d_u(i,j,k)*on_u(i,j)
# else
                cff=0.25_r8*(diff3d_r(i,j,k)+diff3d_r(i-1,j,k))*        &
     &              on_u(i,j)
# endif
#else
                cff=0.25_r8*(diff4(i,j,itrc)+diff4(i-1,j,itrc))*        &
     &              on_u(i,j)
#endif
!>              tl_FX(i,j)=cff*                                         &
!>   &                     ((tl_Hz(i,j,k)+tl_Hz(i-1,j,k))*              &
!>   &                      (dTdx(i,j,k1)-                              &
!>   &                       0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i-1,j,k1)+                &
!>   &                                   dTdr(i  ,j,k2))+               &
!>   &                               MIN(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (dTdr(i-1,j,k2)+                &
!>   &                                   dTdr(i  ,j,k1))))+             &
!>   &                      (Hz(i,j,k)+Hz(i-1,j,k))*                    &
!>   &                      (tl_dTdx(i,j,k1)-                           &
!>   &                       0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i-1,j,k1)+             &
!>   &                                   tl_dTdr(i  ,j,k2))+            &
!>   &                               MIN(dRdx(i,j,k1),0.0_r8)*          &
!>   &                                  (tl_dTdr(i-1,j,k2)+             &
!>   &                                   tl_dTdr(i  ,j,k1)))-           &
!>   &                       0.5_r8*((0.5_r8+                           &
!>   &                                SIGN(0.5_r8, dRdx(i,j,k1)))*      &
!>   &                               tl_dRdx(i,j,k1)*                   &
!>   &                               (dTdr(i-1,j,k1)+dTdr(i,j,k2))+     &
!>   &                               (0.5_r8+                           &
!>   &                                SIGN(0.5_r8,-dRdx(i,j,k1)))*      &
!>   &                               tl_dRdx(i,j,k1)*                   &
!>   &                               (dTdr(i-1,j,k2)+dTdr(i,j,k1)))))
!>
                adfac=cff*ad_FX(i,j)
                adfac1=adfac*(dTdx(i,j,k1)-                             &
     &                        0.5_r8*(MAX(dRdx(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i-1,j,k1)+               &
     &                                    dTdr(i  ,j,k2))+              &
     &                                MIN(dRdx(i,j,k1),0.0_r8)*         &
     &                                   (dTdr(i-1,j,k2)+               &
     &                                    dTdr(i  ,j,k1))))
                adfac2=adfac*(Hz(i,j,k)+Hz(i-1,j,k))
                adfac3=adfac2*0.5_r8*MAX(dRdx(i,j,k1),0.0_r8)
                adfac4=adfac2*0.5_r8*MIN(dRdx(i,j,k1),0.0_r8)
                ad_Hz(i-1,j,k)=ad_Hz(i-1,j,k)+adfac1
                ad_Hz(i  ,j,k)=ad_Hz(i  ,j,k)+adfac1
                ad_dTdx(i,j,k1)=ad_dTdx(i,j,k1)+adfac2
                ad_dTdr(i-1,j,k1)=ad_dTdr(i-1,j,k1)-adfac3
                ad_dTdr(i  ,j,k2)=ad_dTdr(i  ,j,k2)-adfac3
                ad_dTdr(i-1,j,k2)=ad_dTdr(i-1,j,k2)-adfac4
                ad_dTdr(i  ,j,k1)=ad_dTdr(i  ,j,k1)-adfac4
                ad_dRdx(i,j,k1)=ad_dRdx(i,j,k1)-                        &
     &                          adfac2*0.5_r8*                          &
     &                          ((0.5_r8+SIGN(0.5_r8, dRdx(i,j,k1)))*   &
     &                           (dTdr(i-1,j,k1)+dTdr(i,j,k2))+         &
     &                           (0.5_r8+SIGN(0.5_r8,-dRdx(i,j,k1)))*   &
     &                           (dTdr(i-1,j,k2)+dTdr(i,j,k1)))
                ad_FX(i,j)=0.0_r8
              END DO
            END DO
          END IF
          IF ((k.eq.0).or.(k.eq.N(ng))) THEN
            DO j=-1+Jmin,Jmax+1
              DO i=-1+Imin,Imax+1
!>              tl_dTdr(i,j,k2)=0.0_r8
!>
                ad_dTdr(i,j,k2)=0.0_r8
!>              tl_FS(i,j,k2)=0.0_r8
!>
                ad_FS(i,j,k2)=0.0_r8
              END DO
            END DO
          ELSE
            DO j=-1+Jmin,Jmax+1
              DO i=-1+Imin,Imax+1
#if defined TS_MIX_MAX_SLOPE
                cff1=SQRT(dRdx(i,j,k2)**2+dRdx(i+1,j,k2)**2+            &
     &                    dRdx(i,j,k1)**2+dRdx(i+1,j,k1)**2+            &
     &                    dRde(i,j,k2)**2+dRde(i,j+1,k2)**2+            &
     &                    dRde(i,j,k1)**2+dRde(i,j+1,k1)**2)
                cff2=0.25_r8*slope_max*                                 &
     &               (z_r(i,j,k+1)-z_r(i,j,k))*cff1
                cff3=MAX(pden(i,j,k)-pden(i,j,k+1),small)
                cff4=MAX(cff2,cff3)
                cff=-1.0_r8/cff4
#elif defined TS_MIX_MIN_STRAT
                cff1=MAX(pden(i,j,k)-pden(i,j,k+1),                     &
     &                   strat_min*(z_r(i,j,k+1)-z_r(i,j,k)))
                cff=-1.0_r8/cff1
#else
                cff1=MAX(pden(i,j,k)-pden(i,j,k+1),eps)
                cff=-1.0_r8/cff1
#endif
!>              tl_FS(i,j,k2)=tl_cff*(z_r(i,j,k+1)-                     &
!>   &                                z_r(i,j,k  ))+                    &
!>   &                        cff*(tl_z_r(i,j,k+1)-                     &
!>   &                             tl_z_r(i,j,k  ))
!>
                adfac=cff*ad_FS(i,j,k2)
                ad_z_r(i,j,k  )=ad_z_r(i,j,k  )-adfac
                ad_z_r(i,j,k+1)=ad_z_r(i,j,k+1)+adfac
                ad_cff=ad_cff+(z_r(i,j,k+1)-                            &
     &                         z_r(i,j,k  ))*ad_FS(i,j,k2)
                ad_FS(i,j,k2)=0.0_r8
#if defined TS_MIX_STABILITY
!>              tl_dTdr(i,j,k2)=tl_cff*                                 &
!>   &                          (0.75_r8*(t(i,j,k+1,nrhs,itrc)-         &
!>   &                                    t(i,j,k  ,nrhs,itrc))+        &
!>   &                           0.25_r8*(t(i,j,k+1,nstp,itrc)-         &
!>   &                                    t(i,j,k  ,nstp,itrc)))+       &
!>   &                          cff*
!>   &                          (0.75_r8*(tl_t(i,j,k+1,nrhs,itrc)-      &
!>   &                                    tl_t(i,j,k  ,nrhs,itrc))+     &
!>   &                           0.25_r8*(tl_t(i,j,k+1,nstp,itrc)-      &
!>   &                                    tl_t(i,j,k  ,nstp,itrc)))
!>
                adfac=cff*ad_dTdr(i,j,k2)
                adfac1=adfac*0.75_r8
                adfac2=adfac*0.25_r8
                ad_t(i,j,k  ,nrhs,itrc)=ad_t(i,j,k  ,nrhs,itrc)-adfac1
                ad_t(i,j,k+1,nrhs,itrc)=ad_t(i,j,k+1,nrhs,itrc)+adfac1
                ad_t(i,j,k  ,nstp,itrc)=ad_t(i,j,k  ,nstp,itrc)-adfac2
                ad_t(i,j,k+1,nstp,itrc)=ad_t(i,j,k+1,nstp,itrc)+adfac2
                ad_cff=ad_cff+(0.75_r8*(t(i,j,k+1,nrhs,itrc)-           &
     &                                  t(i,j,k  ,nrhs,itrc))+          &
     &                         0.25_r8*(t(i,j,k+1,nstp,itrc)-           &
     &                                  t(i,j,k  ,nstp,itrc)))*         &
     &                        ad_dTdz(i,j,k2)
                ad_dTdz(i,j,k2)=0.0_r8
#elif defined TS_MIX_CLIMA
                IF (LtracerCLM(itrc,ng)) THEN
!>                tl_dTdr(i,j,k2)=tl_cff*((t(i,j,k+1,nrhs,itrc)-        &
!>   &                                     tclm(i,j,k+1,itrc))-         &
!>   &                                    (t(i,j,k  ,nrhs,itrc)-        &
!>   &                                     tclm(i,j,k  ,itrc)))+        &
!>   &                            cff*(tl_t(i,j,k+1,nrhs,itrc)-         &
!>   &                                 tl_t(i,j,k  ,nrhs,itrc))
!>
                  adfac=cff*ad_dTdr(i,j,k2)
                  ad_t(i,j,k  ,nrhs,itrc)=ad_t(i,j,k  ,nrhs,itrc)-adfac
                  ad_t(i,j,k+1,nrhs,itrc)=ad_t(i,j,k+1,nrhs,itrc)+adfac
                  ad_cff=ad_cff+((t(i,j,k+1,nrhs,itrc)-                 &
     &                            tclm(i,j,k+1,itrc))-                  &
     &                           (t(i,j,k  ,nrhs,itrc)-                 &
     &                            tclm(i,j,k  ,itrc)))*ad_dTdr(i,j,k2)
                  ad_dTdr(i,j,k2)=0.0_r8
                ELSE
!>                tl_dTdr(i,j,k2)=tl_cff*(t(i,j,k+1,nrhs,itrc)-         &
!>   &                                    t(i,j,k  ,nrhs,itrc))+        &
!>   &                            cff*(tl_t(i,j,k+1,nrhs,itrc)-         &
!>   &                                 tl_t(i,j,k  ,nrhs,itrc))
!>
                END IF
#else
!>              tl_dTdr(i,j,k2)=tl_cff*(t(i,j,k+1,nrhs,itrc)-           &
!>   &                                  t(i,j,k  ,nrhs,itrc))+          &
!>   &                          cff*(tl_t(i,j,k+1,nrhs,itrc)-           &
!>   &                               tl_t(i,j,k  ,nrhs,itrc))
!>
                adfac=cff*ad_dTdr(i,j,k2)
                ad_t(i,j,k  ,nrhs,itrc)=ad_t(i,j,k  ,nrhs,itrc)-adfac
                ad_t(i,j,k+1,nrhs,itrc)=ad_t(i,j,k+1,nrhs,itrc)+adfac
                ad_cff=ad_cff+(t(i,j,k+1,nrhs,itrc)-                    &
     &                         t(i,j,k  ,nrhs,itrc))*ad_dTdr(i,j,k2)
                ad_dTdr(i,j,k2)=0.0_r8
#endif
#if defined TS_MIX_MAX_SLOPE
!>              tl_cff=cff*cff*tl_cff4
!>
                ad_cff4=ad_cff4+cff*cff*ad_cff
                ad_cff=0.0_r8
!>              tl_cff4=(0.5_r8+SIGN(0.5_r8,cff2-cff3))*tl_cff2+        &
!>   &                  (0.5_r8-SIGN(0.5_r8,cff2-cff3))*tl_cff3
!>
                ad_cff3=ad_cff3+                                        &
     &                  (0.5_r8-SIGN(0.5_r8,cff2-cff3))*ad_cff4
                ad_cff2=ad_cff2+                                        &
     &                  (0.5_r8+SIGN(0.5_r8,cff2-cff3))*ad_cff4
                ad_cff4=0.0_r8
!>              tl_cff3=(0.5_r8+SIGN(0.5_r8,pden(i,j,k)-pden(i,j,k+1)-  &
!>   &                                      small))*                    &
!>   &                  (tl_pden(i,j,k)-tl_pden(i,j,k+1))
!>
                adfac=(0.5_r8+SIGN(0.5_r8,pden(i,j,k)-pden(i,j,k+1)-    &
     &                                    small))*ad_cff3
                ad_pden(i,j,k  )=ad_pden(i,j,k  )+adfac
                ad_pden(i,j,k+1)=ad_pden(i,j,k+1)-adfac
                ad_cff3=0.0_r8
!>              tl_cff2=0.25_r8*slope_max*                              &
!>   &                  ((tl_z_r(i,j,k+1)-tl_z_r(i,j,k))*cff1+          &
!>   &                   (z_r(i,j,k+1)-z_r(i,j,k))*tl_cff1)
!>
                adfac=0.25_r8*slope_max*ad_cff2
                adfac1=adfac*cff1
                ad_cff1=ad_cff1+(z_r(i,j,k+1)-z_r(i,j,k))*adfac
                ad_z_r(i,j,k  )=ad_z_r(i,j,k  )-adfac1
                ad_z_r(i,j,k+1)=ad_z_r(i,j,k+1)+adfac1
                ad_cff2=0.0_r8
                IF (cff1.ne.0.0_r8) THEN
!>                tl_cff1=(dRdx(i  ,j,k2)*tl_dRdx(i  ,j,k2)+            &
!>   &                     dRdx(i+1,j,k2)*tl_dRdx(i+1,j,k2)+            &
!>   &                     dRdx(i  ,j,k1)*tl_dRdx(i  ,j,k1)+            &
!>   &                     dRdx(i+1,j,k1)*tl_dRdx(i+1,j,k1)+            &
!>   &                     dRde(i,j  ,k2)*tl_dRde(i,j  ,k2)+            &
!>   &                     dRde(i,j+1,k2)*tl_dRde(i,j+1,k2)+            &
!>   &                     dRde(i,j  ,k1)*tl_dRde(i,j  ,k1)+            &
!>   &                     dRde(i,j+1,k1)*tl_dRde(i,j+1,k1))/cff1
!>
                  adfac=ad_cff1/cff1
                  ad_dRdx(i  ,j,k1)=ad_dRdx(i  ,j,k1)+                  &
     &                              dRdx(i  ,j,k1)*adfac
                  ad_dRdx(i+1,j,k1)=ad_dRdx(i+1,j,k1)+                  &
     &                              dRdx(i+1,j,k1)*adfac
                  ad_dRdx(i  ,j,k2)=ad_dRdx(i  ,j,k2)+                  &
     &                              dRdx(i  ,j,k2)*adfac
                  ad_dRdx(i+1,j,k2)=ad_dRdx(i+1,j,k2)+                  &
     &                              dRdx(i+1,j,k2)*adfac
                  ad_dRde(i,j  ,k2)=ad_dRde(i,j  ,k2)+                  &
     &                              dRde(i,j  ,k2)*adfac
                  ad_dRde(i,j+1,k2)=ad_dRde(i,j+1,k2)+                  &
     &                              dRde(i,j+1,k2)*adfac
                  ad_dRde(i,j  ,k1)=ad_dRde(i,j  ,k1)+                  &
     &                              dRde(i,j  ,k1)*adfac
                  ad_dRde(i,j+1,k1)=ad_dRde(i,j+1,k1)+                  &
     &                              dRde(i,j+1,k1)*adfac
                  ad_cff1=0.0_r8
                ELSE
!>                tl_cff1=0.0_r8
!>
                  ad_cff1=0.0_r8
                END IF
#elif defined TS_MIX_MIN_STRAT
!>              tl_cff=cff*cff*tl_cff1
!>
                ad_cff1=ad_cff1+cff*cff*ad_cff
                ad_cff=0.0_r8
!>              tl_cff1=(0.5_r8+SIGN(0.5_r8,                            &
!>   &                               pden(i,j,k)-pden(i,j,k+1)-         &
!>   &                               strat_min*(z_r(i,j,k+1)-           &
!>   &                                          z_r(i,j,k  ))))*        &
!>   &                  (tl_pden(i,j,k)-tl_pden(i,j,k+1))+              &
!>   &                  (0.5_r8-SIGN(0.5_r8,                            &
!>   &                               pden(i,j,k)-pden(i,j,k+1)-         &
!>   &                               strat_min*(z_r(i,j,k+1)-           &
!>   &                                          z_r(i,j,k  ))))*        &
!>   &                  (strat_min*(tl_z_r(i,j,k+1)-tl_z_r(i,j,k  )))
!>
                adfac1=(0.5_r8+SIGN(0.5_r8,                             &
     &                              pden(i,j,k)-pden(i,j,k+1)-          &
     &                              strat_min*(z_r(i,j,k+1)-            &
     &                                         z_r(i,j,k  ))))*         &
     &                 ad_cff1
                adfac2=(0.5_r8-SIGN(0.5_r8,                             &
     &                              pden(i,j,k)-pden(i,j,k+1)-          &
     &                              strat_min*(z_r(i,j,k+1)-            &
     &                                         z_r(i,j,k  ))))*         &
     &                 strat_min*ad_cff1
                ad_pden(i,j,k  )=ad_pden(i,j,k  )+adfac1
                ad_pden(i,j,k+1)=ad_pden(i,j,k+1)-adfac1
                ad_z_r(i,j,k  )=ad_z_r(i,j,k  )-adfac2
                ad_z_r(i,j,k+1)=ad_z_r(i,j,k+1)+adfac2
                ad_cff1=0.0_r8
#else
!>              tl_cff=cff*cff*tl_cff1
!>
                ad_cff1=ad_cff1+cff*cff*ad_cff
                ad_cff=0.0_r8
!>              tl_cff1=(0.5_r8+SIGN(0.5_r8,                            &
!>   &                               pden(i,j,k)-pden(i,j,k+1)-eps))*   &
!>   &                  (tl_pden(i,j,k)-tl_pden(i,j,k+1))
!>
                adfac=(0.5_r8+SIGN(0.5_r8,                              &
     &                             pden(i,j,k)-pden(i,j,k+1)-eps))*     &
     &                ad_cff1
                ad_pden(i,j,k  )=ad_pden(i,j,k  )+adfac
                ad_pden(i,j,k+1)=ad_pden(i,j,k+1)-adfac
                ad_cff1=0.0_r8
#endif
              END DO
            END DO
          END IF
          IF (k.lt.N(ng)) THEN
            DO j=Jmin,Jmax+1
              DO i=Imin,Imax
                cff=0.5_r8*(pn(i,j)+pn(i,j-1))
#ifdef MASKING
                cff=cff*vmask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                cff=cff*vmask_wet(i,j)
#endif
#if defined TS_MIX_STABILITY
!>              tl_dTde(i,j,k2)=cff*                                    &
!>   &                          (0.75_r8*(tl_t(i,j  ,k+1,nrhs,itrc)-    &
!>   &                                    tl_t(i,j-1,k+1,nrhs,itrc))+   &
!>   &                           0.25_r8*(tl_t(i,j  ,k+1,nstp,itrc)-    &
!>   &                                    tl_t(i,j-1,k+1,nstp,itrc)))
!>
                adfac=cff*ad_dTde(i,j,k2)
                adfac1=adfac*0.75_r8
                adfac2=adfac*0.25_r8
                ad_t(i,j-1,k+1,nrhs,itrc)=ad_t(i,j-1,k+1,nrhs,itrc)-    &
     &                                    adfac1
                ad_t(i,j  ,k+1,nrhs,itrc)=ad_t(i,j  ,k+1,nrhs,itrc)+    &
     &                                    adfac1
                ad_t(i,j-1,k+1,nstp,itrc)=ad_t(i,j-1,k+1,nstp,itrc)-    &
     &                                    adfac2
                ad_t(i,j  ,k+1,nstp,itrc)=ad_t(i,j  ,k+1,nstp,itrc)+    &
     &                                    adfac2
                ad_dTde(i,j,k2)=0.0_r8
#elif defined TS_MIX_CLIMA
!>              tl_dTde(i,j,k2)=cff*(tl_t(i,j  ,k+1,nrhs,itrc)-         &
!>   &                               tl_t(i,j-1,k+1,nrhs,itrc))
!>
                adfac=cff*ad_dTde(i,j,k2)
                ad_t(i,j-1,k+1,nrhs,itrc)=ad_t(i,j-1,k+1,nrhs,itrc)-    &
     &                                    adfac
                ad_t(i,j  ,k+1,nrhs,itrc)=ad_t(i,j  ,k+1,nrhs,itrc)+    &
     &                                    adfac
                ad_dTde(i,j,k2)=0.0_r8
#else
!>              tl_dTde(i,j,k2)=cff*(tl_t(i,j  ,k+1,nrhs,itrc)-         &
!>   &                               tl_t(i,j-1,k+1,nrhs,itrc))
!>
                adfac=cff*ad_dTde(i,j,k2)
                ad_t(i,j-1,k+1,nrhs,itrc)=ad_t(i,j-1,k+1,nrhs,itrc)-    &
     &                                    adfac
                ad_t(i,j  ,k+1,nrhs,itrc)=ad_t(i,j  ,k+1,nrhs,itrc)+    &
     &                                    adfac
                ad_dTde(i,j,k2)=0.0_r8
#endif
!>              tl_dRde(i,j,k2)=cff*(tl_pden(i,j  ,k+1)-                &
!>   &                               tl_pden(i,j-1,k+1))
!>
                adfac=cff*ad_dRde(i,j,k2)
                ad_pden(i,j-1,k+1)=ad_pden(i,j-1,k+1)-adfac
                ad_pden(i,j  ,k+1)=ad_pden(i,j  ,k+1)+adfac
                ad_dRde(i,j,k2)=0.0_r8
              END DO
            END DO
            DO j=Jmin,Jmax
              DO i=Imin,Imax+1
                cff=0.5_r8*(pm(i,j)+pm(i-1,j))
#ifdef MASKING
                cff=cff*umask(i,j)
#endif
#ifdef WET_DRY_NOT_YET
                cff=cff*umask_wet(i,j)
#endif
#if defined TS_MIX_STABILITY
!>              tl_dTdx(i,j,k2)=cff*                                    &
!>   &                          (0.75_r8*(tl_t(i  ,j,k+1,nrhs,itrc)-    &
!>   &                                    tl_t(i-1,j,k+1,nrhs,itrc))+   &
!>   &                           0.25_r8*(tl_t(i  ,j,k+1,nstp,itrc)-    &
!>   &                                    tl_t(i-1,j,k+1,nstp,itrc)))
!>
                adfac=cff*ad_dTdx(i,j,k2)
                adfac1=adfac*0.75_r8
                adfac2=adfac*0.25_r8
                ad_t(i-1,j,k+1,nrhs,itrc)=ad_t(i-1,j,k+1,nrhs,itrc)-    &
     &                                    adfac1
                ad_t(i  ,j,k+1,nrhs,itrc)=ad_t(i  ,j,k+1,nrhs,itrc)+    &
     &                                    adfac1
                ad_t(i-1,j,k+1,nstp,itrc)=ad_t(i-1,j,k+1,nstp,itrc)-    &
     &                                    adfac2
                ad_t(i  ,j,k+1,nstp,itrc)=ad_t(i  ,j,k+1,nstp,itrc)+    &
     &                                    adfac2
                ad_dTdx(i,j,k2)=0.0_r8
#elif defined TS_MIX_CLIMA
!>              tl_dTdx(i,j,k2)=cff*(tl_t(i  ,j,k+1,nrhs,itrc)-         &
!>   &                               tl_t(i-1,j,k+1,nrhs,itrc))
!>
                adfac=cff*ad_dTdx(i,j,k2)
                ad_t(i-1,j,k+1,nrhs,itrc)=ad_t(i-1,j,k+1,nrhs,itrc)-    &
     &                                    adfac
                ad_t(i  ,j,k+1,nrhs,itrc)=ad_t(i  ,j,k+1,nrhs,itrc)+    &
     &                                    adfac
                ad_dTdx(i,j,k2)=0.0_r8
#else
!>              tl_dTdx(i,j,k2)=cff*(tl_t(i  ,j,k+1,nrhs,itrc)-         &
!>   &                               tl_t(i-1,j,k+1,nrhs,itrc))
!>
                adfac=cff*ad_dTdx(i,j,k2)
                ad_t(i-1,j,k+1,nrhs,itrc)=ad_t(i-1,j,k+1,nrhs,itrc)-    &
     &                                    adfac
                ad_t(i  ,j,k+1,nrhs,itrc)=ad_t(i  ,j,k+1,nrhs,itrc)+    &
     &                                    adfac
                ad_dTdx(i,j,k2)=0.0_r8
#endif
!>              tl_dRdx(i,j,k2)=cff*(tl_pden(i  ,j,k+1)-                &
!>   &                               tl_pden(i-1,j,k+1))
!>
                adfac=cff*ad_dRdx(i,j,k2)
                ad_pden(i-1,j,k+1)=ad_pden(i-1,j,k+1)-adfac
                ad_pden(i  ,j,k+1)=ad_pden(i  ,j,k+1)+adfac
                ad_dRdx(i,j,k2)=0.0_r8
              END DO
            END DO
          END IF
!
!  Compute new storage recursive indices.
!
          kt=k2
          k2=k1
          k1=kt
        END DO K_LOOP3
      END DO T_LOOP
!
      RETURN
      END SUBROUTINE ad_t3dmix4_tile
